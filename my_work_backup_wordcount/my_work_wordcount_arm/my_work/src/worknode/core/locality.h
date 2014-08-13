/*========================================================================
#   FileName: locality.h
#     Author: wangjian
#      Email: kuangrenyx@qq.com
#    Version: 1.0.0
#   HomePage: http://www.xxxxxxxxx.cn
# LastChange: 2013-10-24 12:14:04
========================================================================*/


#ifndef LOCALITY_H_
#define LOCALITY_H_

#include <assert.h>
#include <unistd.h>

#if defined(_LINUX_) && defined(NUMA_SUPPORT)
#include <numaif.h>
#include <numa.h>
#elif defined (_SOLARIS_) && defined(NUMA_SUPPORT)
#include <sys/lgrp_user.h>
#include <sys/mman.h>
#else
// No NUMA support by default for now...
//#warning "NUMA support disabled."
#endif

#include "stddefines.h"
#include "processor.h"

/* Retrieve the number of total locality groups on system. */
inline int loc_get_num_lgrps ()
{
#if defined(_LINUX_) && defined(NUMA_SUPPORT)
    static int max = -1;
    if(max < 0)
        max = numa_max_node() + 1;
    return max;
#elif defined (_SOLARIS_) && defined(NUMA_SUPPORT)
    int ret;
    lgrp_cookie_t cookie;
    int nlgrps;

    cookie = lgrp_init (LGRP_VIEW_CALLER);
    nlgrps = lgrp_nlgrps (cookie);
    ret = lgrp_fini (cookie);
    assert (!ret);

    if (nlgrps > 1)
    {
        /* Do not count the locality group that encompasses all the 
           locality groups. */
        nlgrps -= 1;
    }

    return nlgrps;
#else
    return 1;
#endif
}

/* Retrieve the locality group of the calling LWP. */
inline int loc_get_lgrp ()
{
#if defined(_LINUX_) && defined(NUMA_SUPPORT)
    // assumes locality groups are adjacent and the same size. 
    // Need to figure out if there is a better way to get this info.
    return proc_get_cpuid() / (proc_get_num_cpus() / loc_get_num_lgrps());
#elif defined (_SOLARIS_) && defined(NUMA_SUPPORT)
    int lgrp = lgrp_home (P_LWPID, P_MYID);

    if (lgrp > 0) {
        /* On a system with multiple locality groups, there exists a
           mother locality group (lgroup 0) that encompasses all the 
           locality groups. Collapse down the hierarchy. */
        lgrp -= 1;
    }
    
    return lgrp;
#else
    return -1;
#endif
}

/* Retrieve the locality group of the physical memory that backs
   the virtual address ADDR. */
inline int loc_mem_to_lgrp (void const* addr)
{
#if defined(_LINUX_) && defined(NUMA_SUPPORT)
    int mode;
    get_mempolicy(&mode, NULL, 0, (void*)addr, MPOL_F_NODE | MPOL_F_ADDR);
    return mode;
#elif defined(_SOLARIS_) && defined(NUMA_SUPPORT)
    uint_t info = MEMINFO_VLGRP;
    uint64_t inaddr;
    uint64_t lgrp;
    uint_t validity;

    if (sizeof (void *) == 4) {
        /* 32 bit. */
        inaddr = 0xffffffff & (intptr_t)addr;
    } else {
        /* 64 bit. */
        assert (sizeof (void *) == 8);
        inaddr = (uint64_t)addr;
    }

    CHECK_ERROR(meminfo (&inaddr, 1, &info, 1, &lgrp, &validity));
    
    validity = 1;
    if (validity != 3)
    {
        /* VALIDITY better be 3 here. 
           If it is 1, it means the memory has been assigned, but
           not allocated yet. */
        lgrp = 1;
    }

    if (lgrp > 0) {
        /* On a system with multiple locality groups, there exists a
           mother locality group (lgroup 0) that encompasses all the 
           locality groups. Collapse down the hierarchy. */
        lgrp -= 1;
    }
    
    return lgrp;
#else
    return -1;
#endif
}

#endif /* LOCALITY_H_ */

// vim: ts=8 sw=4 sts=4 smarttab smartindent
