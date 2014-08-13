/*========================================================================
#   FileName: master_control.cpp
#     Author: wangjian
#      Email: kuangrenyx@qq.com
#    Version: 1.0.0
#   HomePage: http://www.xxxxxxxxx.cn
# LastChange: 2013-06-07 16:41:12
========================================================================*/
		
#include <string.h>
#include <unistd.h>
#include "master_control.h"
#include "system_debug.h"
#include "env_manager.h"
#include "memory.h"
		

MasterControl::MasterControl(master_control_arg *arg, MPI_Comm comm_arg)
{
	exitflag = 0;

	pthread_mutex_init(&pMutex, NULL);
	pthread_cond_init(&pCond, NULL);

	mca = arg;
	comm = comm_arg;

	SYS_DEBUG("In MasterControl, nfile is %d", mca->nfile);
	filetask_manager 	= new FileTaskManager();

	int nprocs;

	MPI_Comm_size(comm, &nprocs);
	SYS_DEBUG("MasterControl find nprocs is %d\r\n", nprocs);
	scheduler			= new Scheduler(nprocs);
}


MasterControl::~MasterControl()
{
}
	

void *MasterControl::func_splitter(void *arg)
{
	MasterControl *mc = (MasterControl*)arg;
	mc->do_splitter();

	SYS_DEBUG("Master Control exec splitter over");

	pthread_exit(NULL);
}

void *MasterControl::func_dispatcher(void *arg)
{
	MasterControl *mc = (MasterControl*)arg;
	mc->do_dispatcher();

	SYS_DEBUG("Master Control exec dispatcher over");

	pthread_exit(NULL);
}

void *MasterControl::func_monitor(void *arg)
{
	MasterControl *mc = (MasterControl*)arg;
	mc->do_monitor();

	SYS_DEBUG("Master Control exec dispatcher over");

	pthread_exit(NULL);
}

void MasterControl::do_splitter()
{
#if 0
	pthread_mutex_lock(&pMutex);
	SYS_DEBUG("In func_splitter wait cond");
	pthread_cond_wait(&pCond, &pMutex);
	pthread_mutex_unlock(&pMutex);
#endif
	SYS_DEBUG("In func_splitter pthread start run, nfile is %d", mca->nfile);

	EnvManager::master_control = this;
	for(int i = 0; i < mca->nfile; i++)
	{
		SYS_INFO("splitfile %s", mca->files[i]);
		mca->split_file_callback(mca->files[i]);
	}

	filetask_manager->set_notask_add();
}


void MasterControl::do_dispatcher()
{
#if 0
	pthread_mutex_lock(&pMutex);
	pthread_cond_wait(&pCond, &pMutex);
	SYS_DEBUG("In func_dispatcher wait cond");
	pthread_mutex_unlock(&pMutex);
#endif
	SYS_DEBUG("func_dispatcher pthread start run");
	
	filechunk   *fc;
	int			proc;
	int			count = 0;

	sleep(1);
	while(true)
	{
#if 1
		fc = filetask_manager->gettask();
		if(fc != NULL)
		{
			//SYS_INFO("recv: %s",fc->filename);
			count = 0;
			while(true)
			{
				proc = scheduler->getNextWorkProc();		


				if(proc != -1)
				{
					SYS_INFO("MPI_send filechunk %s to proc %d\r\n",fc->filename, proc);
					MPI_Ssend(fc, sizeof(filechunk), MPI_CHAR, proc, 0x23456, comm);
					break;
				}
				else
				{
					usleep(200000);
					continue;
				}
#if 0
				if(proc == -1)
				{
					//SYS_DEBUG("fuck get no worknode");
					//usleep(1000);
					MPI_Status status;
					int fromwho;

					//SYS_DEBUG("do_monitor before recv work done...");
					SYS_DEBUG("do_dispatch wait response message from proc %d\r\n", fromwho);
					MPI_Recv(&fromwho, 1, MPI_INT, MPI_ANY_SOURCE, 0x12345, comm, &status);
					SYS_DEBUG("do_dispatch recv response message from proc %d\r\n", fromwho);

					if(fromwho <= 0)
						continue;
					else
					{
						SYS_INFO("MPI_send filechunk %s to proc %d\r\n",fc->filename, proc);
						MPI_Send(fc, sizeof(filechunk), MPI_CHAR, proc, 0, comm);
					}

					continue;
				}
				else
				{
					//if(proc == 0)
					{
					SYS_INFO("MPI_send filechunk %s to proc %d\r\n",fc->filename, proc);
					MPI_Send(fc, sizeof(filechunk), MPI_CHAR, proc, 0, comm);
					}
					break;
				}
#endif
			}
		}
#if 1
		else if(filetask_manager->is_notask_add())
		{
			printf("is_notask_add...\r\n");
			usleep(1000);
			count++;
			if(count > 3)
				break;
		}
#endif
#endif
	}


	SYS_INFO("proc 0 dispatch over, sync all node to exit.\r\n");
	while(1)
	{
		proc = scheduler->getNextWorkProc();		

		if(proc != -1)
		{
			filechunk   exitcmd;
			exitcmd.nowork = 1;
			SYS_INFO("MPI_send exitcmd to proc %d\r\n", proc);
			MPI_Send(&exitcmd, sizeof(filechunk), MPI_CHAR, proc, 0, comm);

			scheduler->setWorkProcExit(proc);
		}
		else
		{
			if(scheduler->allWorkDone())
			{
				SYS_INFO("all work done, I can exit...\r\n");
				break;
			}
			else
			{
				//SYS_INFO("wait.............\r\n");
				usleep(200000);
			}
		}
	}
	

	SYS_DEBUG("func_dispatcher pthread run over");
}

void MasterControl::do_monitor()
{
	SYS_DEBUG("func_monitor pthread start run");
	
	//sleep(2);
	while(true)
	{
		MPI_Status status;
		int fromwho[2];

		if(scheduler->allWorkDone1())
		{
			break;
		}

		SYS_DEBUG("do_monitor before recv work done...");
		MPI_Recv(fromwho, 2, MPI_INT, MPI_ANY_SOURCE, 0x12345, comm, &status);
		//MPI_Recv(fromwho, 2, MPI_INT, MPI_ANY_SOURCE, 0, comm, &status);
		SYS_DEBUG("do_monitor recv response message from proc %d type %d\r\n", fromwho[1], fromwho[0]);

		if(fromwho[0] == 2)
		{
			scheduler->setWorkProcExit1(fromwho[1]);
		}
		else if(fromwho[0] == 1)
		{
			if(fromwho[1] < 0)
			{
				continue;
			}
			else
			{
				scheduler->setWorkProcFree(fromwho[1]);
			}
		}
	}

	SYS_DEBUG("func_monitor pthread run over");
}

void MasterControl::addtask(filechunk *fc)
{
	filechunk *file_chunk = (filechunk*)Memory::mem_malloc(sizeof(filechunk));
	memcpy(file_chunk, fc, sizeof(*fc));
	//SYS_DEBUG("\nIn add task %s\n", fc->filename);
	filetask_manager->addtask(file_chunk);
}

void MasterControl::startwork()
{
	//SYS_INFO("start send broadcast");
#if 0
	pthread_mutex_lock(&pMutex);
	pthread_cond_broadcast(&pCond);
	pthread_mutex_unlock(&pMutex);
#endif
	int ret;
	void *retval;

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	//pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	//func_splitter(this);
	ret = pthread_create (&thr_splitter, &attr, func_splitter, this);
	ret = pthread_create (&thr_dispatcher, NULL, func_dispatcher, this);
	ret = pthread_create (&thr_monitor, &attr, func_monitor, this);
	
	//func_dispatcher(this);


	pthread_join(thr_splitter, &retval);
	pthread_join(thr_dispatcher, &retval);
	pthread_join(thr_monitor, &retval);

#if 0
	if(!pthread_cancel(thr_splitter))
	{
		printf("splitter pthread_cancel OK\r\n");
	}
	
	if(!pthread_cancel(thr_monitor))
	{
		printf("splitter pthread_cancel OK\r\n");
	}
#endif

	printf("MasterControl::startwork() execute over...\r\n");
}
		
	    	
	



