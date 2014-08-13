/*========================================================================
#   FileName: work_mapreduce.cpp
#     Author: wangjian
#      Email: kuangrenyx@qq.com
#    Version: 1.0.0
#   HomePage: http://www.xxxxxxxxx.cn
# LastChange: 2013-11-20 11:02:09
========================================================================*/
#include <pthread.h>
#include <unistd.h>
#include "work_mapreduce.h"
#include "env_manager.h"
#include "system_debug.h"
#include "filechunk.h"
//#include "processor.h"
#include "memory.h"


WorkMapReduce::WorkMapReduce(worknode_control_arg *work_arg, MPI_Comm newcomm)
{
	this->work_arg 	= work_arg;
	this->comm		= MPI_COMM_WORLD;
	this->newcomm	= newcomm;

	MPI_Comm_rank(comm, &myproc);
	mapreduce_init();
}

WorkMapReduce::~WorkMapReduce()
{

}

void WorkMapReduce::mapreduce_init()
{
#if 0
	ncores = proc_get_num_cpus();
	SYS_DEBUG("proc %d cores %d", myproc, ncores);

	worktask_mgr 	= new WorkTaskManager(ncores, 1);
	workthr_mgr		= new WorkThreadManager(ncores);

	EnvManager::work_tsk_mgr = worktask_mgr;

	workthr_mgr->threadpool_create();
#endif
}

void WorkMapReduce::do_work()
{
	int msg[2];

	while(true)
	{
		MPI_Status status;
		filechunk fc;

		SYS_INFO("myproc %d wait MPI message...\r\n", myproc);
		MPI_Recv(&fc, sizeof(filechunk), MPI_CHAR, 0, 0x23456, comm, &status);

		if(fc.nowork == 1)
		{
			SYS_INFO("myproc %d recv exit command.\r\n", myproc);
			msg[0] = 2;
			msg[1] = myproc;
			MPI_Send(msg, 2, MPI_INT, 0, 0x12345, comm);
			//usleep(10000);
			break;
		}

		SYS_INFO("myproc %d recv message, filename is %s", myproc, fc.filename);

		SYS_INFO("before init_work_func....\r\n");
		work_arg->do_work_func(&fc, newcomm);
		SYS_INFO("after init_work_func....\r\n");
	
		msg[0] = 1;
		msg[1] = myproc;
		MPI_Send(msg, 2, MPI_INT, 0, 0x12345, comm);
		//break;
		
#if 0
		do_splitter(&fc);

		workthr_mgr->set_thread_func(map_worker);
		
		void **thread_func_args = (void**)Memory::mem_malloc(ncores * sizeof(void*));

		for(int i = 0; i < ncores; i++)
		{
			thread_func_args[i] = this;
		}

		//workthr_mgr->set_thread_func_args(thread_func_args, ncores);

		//SYS_DEBUG("myproc %d begin threads", myproc);
		workthr_mgr->begin_threads(ncores);
		//SYS_DEBUG("myproc %d wait threads over", myproc);
		workthr_mgr->wait_threads();

		SYS_DEBUG("myproc %d send to proc0 over", myproc);
#endif
		//MPI_Send(&myproc, 1, MPI_INT, 0, 0, comm);
	}
}



void WorkMapReduce::do_map_worker(void *thr_arg)
{
	//SYS_INFO("In %s\r\n", __FILE__);
	//WorkMapReduce *wmp = (WorkMapReduce*)arg;
#if 0
	thread_arg 	*arg 	= (thread_arg*)thr_arg;
	workchunk	*wc 	= NULL;
	int 		count 	= 0;
	while(1)
	{
		//SYS_INFO("In %s begore gettask\r\n", __FILE__);
		//wc = worktask_mgr->gettask(arg->thread_index);
		//SYS_INFO("In %s after gettask\r\n", __FILE__);
		if(wc == NULL)
			break;

		count++;
		//SYS_INFO("In %s before  map_func\r\n", __FILE__);
		work_arg->map_func(wc);
	}

	SYS_INFO("In do_map_worker() Proc %d", myproc); 
	SYS_INFO("thread_index  %d", arg->thread_index); 
	SYS_INFO("thread_pid  %u", arg->thread_pid); 
	SYS_INFO("task count  %d", count); 
#endif

}

void* WorkMapReduce::func_startwork(void *thr_arg)
{
	WorkMapReduce *wmp = (WorkMapReduce*)thr_arg;
	wmp->do_work();
}

void WorkMapReduce::startwork()
{
	int ret;

	//ret = pthread_create (&thr_startwork, NULL, func_startwork, this);
	do_work();
}




