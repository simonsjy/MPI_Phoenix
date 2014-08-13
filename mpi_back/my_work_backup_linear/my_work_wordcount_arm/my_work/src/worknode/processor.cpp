/*========================================================================
#   FileName: processor.cpp
#     Author: wangjian
#      Email: kuangrenyx@qq.com
#    Version: 1.0.0
#   HomePage: http://www.xxxxxxxxx.cn
# LastChange: 2013-06-08 08:53:33
========================================================================*/
#include <sched.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <assert.h>

#include "processor.h"

int proc_get_num_cpus (void)
{
    int num_cpus;
    char *num_proc_str;

    num_cpus = sysconf(_SC_NPROCESSORS_ONLN);

    return num_cpus;
}

static cpu_set_t    full_cs;
static cpu_set_t* proc_get_full_set(void)
{
    static int          inited = 0;

    if (inited == 0) {
        int i;
        int n_cpus;

        CPU_ZERO (&full_cs);
        n_cpus = sysconf(_SC_NPROCESSORS_ONLN);
        for (i = 0; i < n_cpus; i++) {
            CPU_SET(i, &full_cs);
        }

        inited = 1;
    }

    return &full_cs;
}

int proc_bind_thread (int cpu_id)
{
    cpu_set_t   cpu_set;

    CPU_ZERO (&cpu_set);
    CPU_SET (cpu_id, &cpu_set);

    return sched_setaffinity (0, sizeof (cpu_set), &cpu_set);
}

int proc_unbind_thread ()
{
    return sched_setaffinity (0, sizeof (cpu_set_t), proc_get_full_set());
}

int proc_is_available (int cpu_id)
{
    int ret;
    cpu_set_t cpu_set;
    
    ret = sched_getaffinity (0, sizeof (cpu_set), &cpu_set);
    if (ret < 0) return 1;

    return CPU_ISSET (cpu_id, &cpu_set) ? 1 : 0;
}

int proc_get_cpuid (void)
{
    int i, ret;
    cpu_set_t cpu_set;
    
    ret = sched_getaffinity (0, sizeof (cpu_set), &cpu_set);
    if (ret < 0) return -1;

    for (i = 0; i < CPU_SETSIZE; ++i)
    {
        if (CPU_ISSET (i, &cpu_set)) break;
    }
    return i;
}
