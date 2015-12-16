#ifndef _CONTAINER_WRAPPER_H_
#define _CONTAINER_WRAPPER_H_

#define MAX_NODES       16
#define MAX_NAME        100
#define MAX_PATH        100
/*FIXME soon: These fixed parameters should be taken from YARN config*/
#define CGROUP_PATH_PREFIX      "/sys/fs/cgroup"
#define CGROUP_HIERARCHY        "/yarn"
#define DEFAULT_STRICT_RESOURCE_POLICY true

#define CONTROLLER_CPU          "cpu"
#define CONTROLLER_MEMORY       "memory"
#define PRINT_FUNC_LINE printf("%s: %d\n",__FUNCTION__,__LINE__);
//#define PRINT_FUNC_LINE do{}while(0)

#endif
