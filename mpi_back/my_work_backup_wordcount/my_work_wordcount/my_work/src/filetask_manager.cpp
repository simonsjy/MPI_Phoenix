/*========================================================================
#   FileName: filetask_manager.cpp
#     Author: wangjian
#      Email: kuangrenyx@qq.com
#    Version: 1.0.0
#   HomePage: http://www.xxxxxxxxx.cn
# LastChange: 2013-06-08 08:55:04
========================================================================*/
#include "filetask_manager.h"
#include "system_debug.h"
#include "memory.h"


FileTaskManager::FileTaskManager()
{
	
	num_task 	= 0;
	head_task	= NULL;
	tail_task	= NULL;
	notask_add	= 0;


	pthread_mutex_init(&pMutex, NULL);
}

FileTaskManager::~FileTaskManager()
{

}

void FileTaskManager::set_notask_add()
{
	notask_add = 1;
}

int FileTaskManager::is_notask_add()
{
	return notask_add;
}

void FileTaskManager::addtask(filechunk *fc)
{
	FileTask *ft = (FileTask*)Memory::mem_malloc(sizeof(FileTask));
	
	ft->fc = fc;
	
	pthread_mutex_lock(&pMutex);
	
	if(num_task == 0)
	{
		head_task = tail_task = &ft->node;
	}

	list_add_head(&ft->node, head_task);
	head_task = &ft->node;
	num_task++;
	
	//SYS_DEBUG("num_task is %d filename %s\r\n", num_task, fc->filename);
	pthread_mutex_unlock(&pMutex);
}


filechunk *FileTaskManager::gettask()
{
	FileTask	*ft = NULL;
	filechunk 	*fc = NULL;

	pthread_mutex_lock(&pMutex);

	if(num_task !=0 && tail_task != NULL)
	{
		ft = list_entry(tail_task, FileTask, node);
		SYS_ASSERT(ft != NULL);

		fc = ft->fc;
		SYS_ASSERT(fc != NULL);
		
		Memory::mem_free(ft);
		tail_task = list_remove_tail(tail_task);
		//SYS_INFO("num_task is %d\r\n", num_task);
		num_task--;
	}

	pthread_mutex_unlock(&pMutex);

	return fc;
}


