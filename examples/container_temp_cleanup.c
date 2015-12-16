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
	
	if(gethostname(hostname,MAX_NAME)){
		perror("gethostname");
		ret = errno;
		goto exit_label;
	}

	MPI_Init(&argc,&argv);
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);

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
#ifdef ENABLE_CGROUPS
	/*Only one process per node does this. Now that we have MPI initialized, it is easy to manage*/
	if(found /*&& (rank % num_containers == 0)*/){
		/*Create a cgroup structure here, with the same name as the one created by YARN
		conainer executor*/
		create_cgroup_path(cgroup_name,containerName[i]);
		/*FIXME: 
		Since currently YARN does not create cgroup folders for memory controller, we need
                to create it manually, instead of only changing the permissions like the cpu controller
                Now we need to delete these memory cgroups after user program is done*/
		chmod_container(sys_cmd,CONTROLLER_MEMORY,cgroup_name,"g+w");
		delete_cgroup_controller(sys_cmd,CONTROLLER_MEMORY,cgroup_name);
	}
#endif
	MPI_Finalize();

exit_label:
	return ret;
}
