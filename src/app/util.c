#include "util.h"

#include <sys/sysinfo.h> 
#include <sys/stat.h> 
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

/******************** Added by Chi ********************/
int get_cpu_cores()
{
    return get_nprocs();
}

float get_cpu_load()
{
    int fd;
    char buffer[64];
    float load;

    fd = open("/proc/loadavg", O_RDONLY);
    if (fd < 0)
        return -1;
    
    read(fd, buffer, sizeof(buffer) - 1);
    sscanf(buffer, "%f", &load);
    close(fd);
    
    return load;
}
/******************************************************/
