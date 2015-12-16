/*This program is used as a container around MPI programs to apply CGroup policies.
Once CGroup work is done, the main MPI program will be executed, which will inherit
CGroups from this program. This program is used inside mpich2-yarn.*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include <libcgroup.h>
#include <mpi.h>

#include "container_wrapper.h"

char *create_cgroup_path(char *cgroup_name,char *name){
	strcpy(cgroup_name,CGROUP_HIERARCHY);
	strcat(cgroup_name,"/");
	strcat(cgroup_name,name);
	return cgroup_name;
}

void chmod_container(char cmd[],char controller[], char cgroup_name[],char *mode_str){
	strcpy(cmd,"sudo chmod -R ");
	strcat(cmd,mode_str);
	strcat(cmd," ");
	strcat(cmd,CGROUP_PATH_PREFIX);
	strcat(cmd,"/");
	strcat(cmd,controller);
        strcat(cmd,cgroup_name);//needs to have a leading slash (/)
        system(cmd);
}

void set_cgroup_parameter(char cmd[],char cgroup_name[],char param[],char val[]){
	strcpy(cmd,"sudo cgset -r ");
        strcat(cmd,param);
	strcat(cmd,"=");
	strcat(cmd,val);
	strcat(cmd," ");
	strcat(cmd,cgroup_name);//needs to have a leading slash (/)
	system(cmd);
}
void create_cgroup_controller(char cmd[],char controller[],char cgroup_name[]){
	strcpy(cmd,"sudo cgcreate -d 775 -f 775 -g ");
	strcat(cmd,controller);
	strcat(cmd,":");
	strcat(cmd,cgroup_name);
	strcat(cmd," -a yarn:hadoop");
	system(cmd);
	strcpy(cmd,"sudo chown yarn:hadoop ");
	strcat(cmd,CGROUP_PATH_PREFIX);
	strcat(cmd,"/");
	strcat(cmd,controller);
	strcat(cmd,cgroup_name);//needs to have a leading slash (/)
	strcat(cmd,"/tasks");
	system(cmd);
}

void delete_cgroup_controller(char cmd[],char controller[],char cgroup_name[]){
	strcpy(cmd,"sudo cgdelete -g ");
	strcat(cmd,controller);
        strcat(cmd,":");
        strcat(cmd,cgroup_name);
	system(cmd);
}

char hostname[MAX_NAME];
char hdfsAddress[MAX_PATH];
char containerHost[MAX_NODES][MAX_NAME];
char containerName[MAX_NODES][MAX_NAME];
char cgroup_name[MAX_PATH];
char sys_cmd[MAX_PATH];

int main(int argc, char *argv[]){
	int found = 0;
	FILE *container_info;
	char *cg_name;
	int num_containers,i = 0;
	int ret = 0;
	
	printf("Starting MPI wrapper program ...\n");
	if(gethostname(hostname,MAX_NAME)){
		perror("gethostname");
		ret = errno;
		goto exit_label;
	}

	char *container_memory = argv[3];
	bool strict_resource = false;
	if(atoi(argv[4]))
		strict_resource = true;
	int mpi_args = 5;

	/*MPI_Init(&argc,&argv);
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);*/

	memset(containerName,0,MAX_NAME * MAX_NODES);
	memset(containerHost,0,MAX_NAME * MAX_NODES);
	strcpy(sys_cmd,"hdfs dfs -copyToLocal ");
	strcat(sys_cmd,argv[1]);
	strcat(sys_cmd," ./containerInfo.txt");
	system(sys_cmd);	
	container_info = fopen("containerInfo.txt","r");
	fscanf(container_info,"%s",hdfsAddress);
	fscanf(container_info,"%d",&num_containers);
	while(!feof(container_info)){
		fscanf(container_info,"%s",containerName[i]);
		fscanf(container_info,"%s",containerHost[i]);
		if(!strlen(containerName[i]) || !strlen(containerHost[i]))
			break;
		i++;
	}
	assert(i == num_containers);
	fclose(container_info);
	
	for(i=0;i<num_containers;i++){
		if(!strcmp(containerHost[i],hostname)){
                               found = 1;
				break;
                }
	}
//FIXME: Check YARN and only do it if CGRoups are enabled
#ifdef ENABLE_CGROUPS
	if(found){
		if(cgroup_init()){
			perror("cgroup_init");
			goto exit_label;
		}
		/*Create a cgroup structure here, with the same name as the one created by YARN
		conainer executor*/
		create_cgroup_path(cgroup_name,containerName[i]);
		chmod_container(sys_cmd,CONTROLLER_CPU,cgroup_name,"g+w");
		/*Since currently YARN does not create cgroup folders for memory controller, we need 
		to create it manually, instead of only changing the permissions like the cpu controller	
		Then we delete these memory cgroups after user program is done*/
		/*FIXME: Deleteing memory cgroups currently happens in ApplicationMaster 
		(launchMpiCleanupWrapper) by launching wrapper program/script container_temp_cleanup 
		(colocated with this program). However,	that eventually needs to be done by YARN. 
		Currently, this method of cleanup of memory CGroups has a bug. If the program is 
		terminated / killed, the cleanup program is not run and memory CGroups for containers
		stay in /sys/fs/cgroup/memory/yarn/ and need to be deleted by hand using:
		 cgdelete -g memory:/yarn/<container_name>*/
		create_cgroup_controller(sys_cmd,CONTROLLER_MEMORY,cgroup_name);
		chmod_container(sys_cmd,CONTROLLER_MEMORY,cgroup_name,"g+w");	
		
		/*skipping the prefix*/
		struct cgroup *cg = cgroup_new_cgroup(cgroup_name);
		if(!cg){
			perror("cgroup_new_cgroup");
			goto exit_label;
		}
		int ret = cgroup_get_cgroup(cg);
		/*Need to have the cgroup owned by the same user of this process*/
		if(ret = cgroup_attach_task(cg)){
			printf("Error in cgroup_attach_task_pid: %d\n",ret);
			goto exit_label;
		}
		if(strict_resource){
			set_cgroup_parameter(sys_cmd,cgroup_name,"memory.limit_in_bytes",container_memory);
		}else{
			set_cgroup_parameter(sys_cmd,cgroup_name,"memory.soft_limit_in_bytes",container_memory);
		}
		chmod_container(sys_cmd,CONTROLLER_CPU,cgroup_name,"g-w");
		chmod_container(sys_cmd,CONTROLLER_MEMORY,cgroup_name,"g-w");
		cgroup_free(&cg);
	}
#endif
	//MPI_Finalize();
	if(execv(argv[2],&argv[mpi_args]))
		perror("Error in execv");

exit_label:
	return ret;
}
