#ifndef _CONTAINER_WRAPPER_H_
#define _CONTAINER_WRAPPER_H_

#define MAX_NODES       16
#define MAX_NAME        256
#define MAX_PATH        256
#define CG_PATH		20	
/*FIXME soon: These fixed parameters should be taken from YARN config
  If needed, kjust do it the same way parameters such as cgroups_enabled
  or strict resource are passed in to container_wrapper
*/
#define CGROUP_PATH_PREFIX      "/sys/fs/cgroup"  //
#define CGROUP_HIERARCHY        "/yarn"

#define CONTROLLER_CPU          "cpu"
#define CONTROLLER_MEMORY       "memory"
#define PRINT_FUNC_LINE printf("%s: %d\n",__FUNCTION__,__LINE__);
//#define PRINT_FUNC_LINE do{}while(0)

#endif
