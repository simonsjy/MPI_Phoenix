/*========================================================================
#   FileName: scheduler.h
#     Author: wangjian
#      Email: kuangrenyx@qq.com
#    Version: 1.0.0
#   HomePage: http://www.xxxxxxxxx.cn
# LastChange: 2013-06-07 14:22:34
========================================================================*/
#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__


#define MAX_NODES 1000

class Scheduler {
	public:
		Scheduler(int);
		~Scheduler();

		int getNextWorkProc();
		int setWorkProcFree(int proc);
		int allWorkDone();
		int allWorkDone1();
		int setWorkProcExit(int proc);
		int setWorkProcExit1(int proc);

	private:
		int unused_procs[MAX_NODES];	//0 indicate not used currently, 1 indicate using
		int exit_procs[MAX_NODES];	//0 indicate not exit, 1 indicate already exit
		int exit1_procs[MAX_NODES];	//0 indicate not exit, 1 indicate already exit
		int curr_num_procs;	 //include Proc 0, but Proc 0 don't join do work
		int curr_index;
};











#endif


