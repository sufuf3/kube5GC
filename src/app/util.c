#include "util.h"

#include <sys/sysinfo.h> 
#include <sys/stat.h> 
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

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

status_t rand_select_non_dup(int haystack_count, int select_count, int *sel_indices)
{
    if (select_count > haystack_count)
        return CORE_ERROR;
    
    hash_t *sel_hash = hash_make();
    int i = 0;
    const int val_set = 1;

    srand(time(0));
    while (i < select_count)
    {
        int *sel = (int *) malloc(sizeof(int));
        *sel = rand() % haystack_count;
        
        if (hash_get(sel_hash, sel, sizeof(int)) == NULL)
        {
            hash_set(sel_hash, sel, sizeof(int), &val_set);
            sel_indices[i++] = *sel;
        }
    }

    hash_destroy(sel_hash);
    return CORE_OK;
}

status_t ht_rand_select_non_dup(hash_t *haystack, int select_count, void **sel_items)
{   
    hash_t *sel_hash = hash_make();
    int haystack_count = hash_count(haystack);
    int i = 0;
    const int val_set = 1;

    srand(time(0));
    while (i < select_count)
    {
        int *sel = (int *) malloc(sizeof(int));
        *sel = rand() % haystack_count;
        
        if (hash_get(sel_hash, sel, sizeof(int)) == NULL)
        {
            hash_set(sel_hash, sel, sizeof(int), &val_set);
            i++;
        }
    }

    hash_index_t *it;
    int sel_index = 0;
    i = 0;
    for (it = hash_first(haystack); it; it = hash_next(it))
    {
        if (hash_get(sel_hash, &i, sizeof(int)) != NULL)
            sel_items[sel_index++] = hash_this_val(it);
        i++;
    }

    hash_destroy(sel_hash);

    return CORE_OK;
}
