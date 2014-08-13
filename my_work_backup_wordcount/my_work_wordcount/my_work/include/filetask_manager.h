/*========================================================================
#   FileName: filetask_manager.h
#     Author: simon
#      Email: 297873898@qq.com
#    Version: 1.0.0
#   HomePage: http://www.xxxxxxxxx.cn
# LastChange: 2014-06-07 11:41:33
========================================================================*/
#ifndef __FILETASK_MANAGER_H__
#define __FILETASK_MANAGER_H__

#include <pthread.h>
#include "list.h"
#include "filechunk.h"


typedef struct FileTask_t {
	filechunk	*fc;
	list_head	node;
}FileTask;


class FileTaskManager {		//FileTaskManager responsible for maintaining the FileTask's list
	public:
		FileTaskManager();
		~FileTaskManager();

		void addtask(filechunk *fc);
		filechunk *gettask();

		void set_notask_add();
		int	 is_notask_add();

	private:
		int 		num_task;
		list_head 	*head_task;		//point the head of FileTask's list
		list_head	*tail_task;		//point the tail of FileTask's list
		int 		notask_add;		//0:split task is not complete, 
									//1:split task complete,all filechunk are  
									//generated and already added to the list

		pthread_mutex_t pMutex;
};



#endif


