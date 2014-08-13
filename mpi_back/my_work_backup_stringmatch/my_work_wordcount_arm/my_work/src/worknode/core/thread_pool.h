/*========================================================================
#   FileName: thread_pool.h
#     Author: wangjian
#      Email: kuangrenyx@qq.com
#    Version: 1.0.0
#   HomePage: http://www.xxxxxxxxx.cn
# LastChange: 2013-10-24 12:16:19
========================================================================*/

#ifndef TPOOL_H_
#define TPOOL_H_

#include "stddefines.h"
#include "synch.h"

typedef void (*thread_func)(void *, thread_loc const& loc);
class sched_policy;

class thread_pool
{
public:
    thread_pool(int num_threads, sched_policy const* policy = NULL);
    ~thread_pool();

    int set(thread_func thread_func, void** args, int num_workers);
    int begin();
    int wait();

private:
    struct thread_arg_t {
        thread_pool*    pool;
        thread_loc      loc;
        semaphore       sem_run;
    };

    int             num_threads;
    int             num_workers;
    int             die;
    thread_func     thread_function;
    semaphore       sem_all_workers_done;
    unsigned int    num_workers_done;
    void            **args;
    pthread_t       *threads;
    thread_arg_t    *thread_args;


    static void* loop (void*);
};

#endif /* TPOOL_H_ */

// vim: ts=8 sw=4 sts=4 smarttab smartindent
