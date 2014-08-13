/*========================================================================
#   FileName: master_control.h
#     Author: wangjian
#      Email: kuangrenyx@qq.com
#    Version: 1.0.0
#   HomePage: http://www.xxxxxxxxx.cn
# LastChange: 2013-06-07 20:11:57
========================================================================*/
#ifndef __MASTER_DISPATCH_H__
#define __MASTER_DISPATCH_H__

#include <pthread.h>
#include <sys/types.h>
#include <mpi.h>
#include "mapreduce.h"
#include "filechunk.h"
#include "scheduler.h"
#include "filetask_manager.h"

class MasterControl {
	public:
		MasterControl(master_control_arg *arg, MPI_Comm comm);
		~MasterControl();
	
		void startwork();
		
		void addtask(filechunk *fc);
		filechunk *gettask();
	
	private:
		master_control_arg *mca;

		pthread_t thr_splitter;
		pthread_t thr_dispatcher;
		pthread_t thr_monitor;
		
		static void *func_splitter(void *arg);
		static void *func_dispatcher(void *arg);
		static void *func_monitor(void *arg);

		void do_splitter();
		void do_dispatcher();
		void do_monitor();

		FileTaskManager *filetask_manager;
		Scheduler		*scheduler;

		pthread_cond_t 	pCond;
		pthread_mutex_t	pMutex;

		MPI_Comm		comm;
		int exitflag;
};

#endif


