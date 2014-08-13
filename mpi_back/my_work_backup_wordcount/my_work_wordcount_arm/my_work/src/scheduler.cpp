/*========================================================================
#   FileName: scheduler.cpp
#     Author: wangjian
#      Email: kuangrenyx@qq.com
#    Version: 1.0.0
#   HomePage: http://www.xxxxxxxxx.cn
# LastChange: 2013-11-02 09:21:10
========================================================================*/
#include <string.h>
#include <stdio.h>
#include "scheduler.h"

Scheduler::Scheduler(int num_procs)
{
	curr_num_procs = num_procs;
	memset(unused_procs, 0x00, num_procs*4);
	memset(exit_procs, 0x00, num_procs*4);
	memset(exit1_procs, 0x00, num_procs*4);
	curr_index = 1;	   //because proc 0 also used to make scheduling
}
		
		
Scheduler::~Scheduler()
{

}

int Scheduler::getNextWorkProc()
{

	curr_index = 1;
	for(curr_index; curr_index < curr_num_procs; curr_index++)
	{
		if(unused_procs[curr_index] == 0)
		{
			unused_procs[curr_index] = 1;
			return curr_index;
		}
	}

	return -1;
}


int Scheduler::allWorkDone()
{
	
	curr_index = 1;
	for(curr_index; curr_index < curr_num_procs; curr_index++)
	{
		if(exit_procs[curr_index] == 0)
		{
			//printf("allWorkDone curr_index %d\r\n", curr_index);
			return 0;
		}
	}

	return 1;
}

int Scheduler::allWorkDone1()
{
	
	curr_index = 1;
	for(curr_index; curr_index < curr_num_procs; curr_index++)
	{
		if(exit1_procs[curr_index] == 0)
		{
			//printf("allWorkDone curr_index %d\r\n", curr_index);
			return 0;
		}
	}

	return 1;
}

int Scheduler::setWorkProcExit(int proc)
{
	exit_procs[proc] = 1;
}

int Scheduler::setWorkProcExit1(int proc)
{
	exit1_procs[proc] = 1;
}

int Scheduler::setWorkProcFree(int proc)
{
	unused_procs[proc] = 0;
}
