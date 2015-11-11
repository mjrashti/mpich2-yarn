#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <libcgroup.h>

#define MAX_NODES	16
#define MAX_NAME	100
#define MAX_PATH	100
#define CGROUP_PATH_PREFIX	"/sys/fs/cgroup"
#define CONTROLLER_CPU		"cpu"
#define CONTROLLER_MEMORY	"memory"
#define PRINT_FUNC_LINE	printf("%s: %d\n",__FUNCTION__,__LINE__);
//#define PRINT_FUNC_LINE do{}while(0)

char *create_cgroup_path(char *cgroup_name,/*char *controller,*/char *name){
        /*strcpy(cgroup_name,CGROUP_PATH_PREFIX);
	strcat(cgroup_name,"/");
	strcat(cgroup_name,controller);*/
	
	strcpy(cgroup_name,"");
	strcat(cgroup_name,"/");
	strcat(cgroup_name,name);
	return cgroup_name;
}

char hostname[MAX_NAME];
char hdfsAddress[MAX_PATH];
char containerHost[MAX_NODES][MAX_NAME];
char containerName[MAX_NODES][MAX_NAME];
char cgroup_name[MAX_PATH];
char hdfs_cmd[MAX_PATH];

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
		
	//MPI_Barrier(MPI_COMM_WORLD);
	memset(containerName,0,MAX_NAME * MAX_NODES);
	memset(containerHost,0,MAX_NAME * MAX_NODES);
	strcpy(hdfs_cmd,"hdfs dfs -copyToLocal ");
	strcat(hdfs_cmd,argv[1]);
	strcat(hdfs_cmd," ./containerInfo.txt")	;
	system(hdfs_cmd);	
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
	if(found){
		if(cgroup_init()){
			perror("cgroup_init");
			goto exit_label;
		}

		create_cgroup_path(cgroup_name/*,CONTROLLER_CPU*/,containerName[i]);	
		struct cgroup *cg = cgroup_new_cgroup(cgroup_name);
		if(!cg){
			perror("cgroup_new_cgroup");
			goto exit_label;
		}
		cgroup_get_cgroup(cg);
		/*Need to have the cgroup owned by the same user of this process*/
		if(ret = cgroup_attach_task(cg)){
			printf("Error in cgroup_attach_task_pid: %d\n",ret);
			goto exit_label;
		}
	}
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
