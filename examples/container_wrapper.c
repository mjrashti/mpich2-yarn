/*This program is used as a container around MPI programs to apply CGroup policies.
Once CGroup work is done, the main MPI program will be executed, which will inherit
CGroups from this program. This program is used inside mpich2-yarn.*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <libcgroup.h>
#include <mpi.h>

#define MAX_NODES	16
#define MAX_NAME	100
#define MAX_PATH	100
/*FIXME soon: These fixed parameters should be taken from YARN config*/
#define CGROUP_PATH_PREFIX	"/sys/fs/cgroup"
#define CGROUP_HIERARCHY	"/yarn"

#define CONTROLLER_CPU		"cpu"
#define CONTROLLER_MEMORY	"memory"
#define PRINT_FUNC_LINE	printf("%s: %d\n",__FUNCTION__,__LINE__);
//#define PRINT_FUNC_LINE do{}while(0)

char *create_cgroup_path(char *cgroup_name,char *name){
	strcpy(cgroup_name,CGROUP_HIERARCHY);
	strcat(cgroup_name,"/");
	strcat(cgroup_name,name);
	return cgroup_name;
}

void chmod_container(char cmd[],char controller[], char cgroup_name[]){
	strcpy(cmd,"sudo chmod -R g+w ");
	strcat(cmd,CGROUP_PATH_PREFIX);
	strcat(cmd,"/");
	strcat(cmd,controller);
        strcat(cmd,cgroup_name);//needs to have a leading slash (/)
        system(cmd);
}

void set_cgroup_parameter(char cmd[],char cgroup_name[],char param[],char val[]){
	strcpy(cmd,"cgset -r ");
        strcat(cmd,param);
	strcat(cmd,"=");
	strcat(cmd,val);
	strcat(cmd," ");
	strcat(cmd,cgroup_name);//needs to have a leading slash (/)
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
	int num_containers,proc_len,i = 0;
	int ret = 0;
	
	printf("Starting MPI wrapper program ...\n");
	if(gethostname(hostname,&proc_len)){
		perror("gethostname");
		ret = errno;
		goto exit_label;
	}

	/*int num_vcores = atoi(argv[2]);
	bool strict_resource = DEFAULT_STRICT_RESOURCE_POLICY;*/

	/*MPI_Init(&argc,&argv);
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);*/

	memset(containerName,0,MAX_NAME * MAX_NODES);
	memset(containerHost,0,MAX_NAME * MAX_NODES);
	strcpy(sys_cmd,"hdfs dfs -copyToLocal ");
	strcat(sys_cmd,argv[1]);
	strcat(sys_cmd," ./containerInfo.txt");
	//printf("HDFS command: %s\n",sys_cmd);
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
#ifdef ENABLE_CGROUPS
	if(found){
		if(cgroup_init()){
			perror("cgroup_init");
			goto exit_label;
		}
		/*Create a cgroup structure here, with the same name as the one created by YARN
		conainer executor*/
		create_cgroup_path(cgroup_name,containerName[i]);
		chmod_container(sys_cmd,CONTROLLER_CPU,cgroup_name);
		/*Memory controller currently not used in cgroups in YARN*/
		//chmod_container(sys_cmd,CONTROLLER_MEMORY,cgroup_name);	
		
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
	}
#endif
	//MPI_Finalize();
	if(argv[3]){
		if(execv(argv[2],&argv[3]) < 0)
			perror("Error in execv");
	}
	else{
		if(execv(argv[2],NULL) < 0)
                        perror("Error in execv");
	}

exit_label:
	return ret;
}
