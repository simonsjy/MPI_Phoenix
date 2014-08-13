/*========================================================================
#   FileName: scheduler.h
#     Author: wangjian
#      Email: kuangrenyx@qq.com
#    Version: 1.0.0
#   HomePage: http://www.xxxxxxxxx.cn
# LastChange: 2013-10-24 12:15:03
========================================================================*/

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "locality.h"
#include "processor.h"


#ifdef _SOLARIS_
#define NUM_CORES_PER_CHIP      8
#define NUM_STRANDS_PER_CORE    8
#endif

class sched_policy
{
protected:
    int     num_cpus;
    int     num_chips_per_sys;
    int        offset;
public:
    sched_policy(int offset = 0) : offset(offset) 
    {
        num_cpus = proc_get_num_cpus();
        num_chips_per_sys = loc_get_num_lgrps ();
    }

    virtual ~sched_policy() {}

    virtual int thr_to_cpu(int thr) const = 0;
};

class sched_policy_strand_fill : public sched_policy
{
public:
    sched_policy_strand_fill(int offset = 0) : sched_policy(offset) {}
    int thr_to_cpu(int thr) const
    {
        return (thr+offset) % num_cpus;
    }
};

class sched_policy_core_fill : public sched_policy
{
public:
    sched_policy_core_fill(int offset = 0) : sched_policy(offset) {}
    int thr_to_cpu(int thr) const
    {
#ifdef NUM_CORES_PER_CHIP
        int core, strand;
        thr += offset;
        thr %= num_cpus;
        core = thr % (NUM_CORES_PER_CHIP * num_chips_per_sys);
        strand = (thr / (NUM_CORES_PER_CHIP * num_chips_per_sys));
        strand %= NUM_STRANDS_PER_CORE;
        return (core * NUM_STRANDS_PER_CORE + strand);
#else
        return (thr+offset) % num_cpus;
#endif
    }
};

class sched_policy_chip_fill : public sched_policy
{
public:
    sched_policy_chip_fill(int offset = 0) : sched_policy(offset) {}
    int thr_to_cpu(int thr) const
    {
#ifdef NUM_CORES_PER_CHIP
        int chip,  core, strand;
        thr += offset;
        thr %= num_cpus;
        chip = thr % num_chips_per_sys;
        core = (thr / num_chips_per_sys) % NUM_CORES_PER_CHIP;
        strand = thr / (NUM_CORES_PER_CHIP * NUM_STRANDS_PER_CORE);
        strand %= NUM_STRANDS_PER_CORE;

        return (chip * (NUM_CORES_PER_CHIP * NUM_STRANDS_PER_CORE) +
                core * (NUM_STRANDS_PER_CORE) +
                strand);
#else
        return (thr+offset) % num_cpus;
#endif
    }
};

#endif /* SCHEDULER_H_ */

// vim: ts=8 sw=4 sts=4 smarttab smartindent
