/*========================================================================
#   FileName: work_mapreduce.h
#     Author: wangjian
#      Email: kuangrenyx@qq.com
#    Version: 1.0.0
#   HomePage: http://www.xxxxxxxxx.cn
# LastChange: 2013-06-07 18:20:17
========================================================================*/
#ifndef __WORK_MAPREDUCE_H__
#define __WORK_MAPREDUCE_H__


#include <mpi.h>
#include <pthread.h>
#include "filechunk.h"
#include "iterator.h"

typedef int	  (*do_work_t)(filechunk *fc, MPI_Comm newcomm);

typedef struct worknode_control_arg_t {

	do_work_t 	do_work_func;

}worknode_control_arg;

class WorkMapReduce {
	public:
		WorkMapReduce(worknode_control_arg *work_arg, MPI_Comm comm);
		~WorkMapReduce();

		void mapreduce_init();
		void startwork();
		void do_work();
		void do_map_worker(void *thr_arg);

		static void *func_startwork(void *arg);

	private:
		MPI_Comm				comm;
		MPI_Comm				newcomm;
		int 					ncores;
		int 					myproc;

		pthread_t				thr_startwork;

		worknode_control_arg	*work_arg;
};



#endif
