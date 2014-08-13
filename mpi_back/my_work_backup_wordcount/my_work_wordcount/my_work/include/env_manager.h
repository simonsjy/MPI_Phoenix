/*========================================================================
#   FileName: env_manager.h
#     Author: simon
#      Email: 297873898x@qq.com
#    Version: 1.0.0
#   HomePage: http://www.xxxxxxxxx.cn
# LastChange: 2014-05-10 16:31:59
========================================================================*/
#ifndef __ENV_MANAGER_H__
#define __ENV_MANAGER_H__

#include "master_control.h"
//#include "worktask_manager.h"
#include "filechunk.h"
//#include "workchunk.h"


class EnvManager {		
	public:
		EnvManager();
		~EnvManager();
		
		static void add_filechunk(filechunk *fc);
		//static void add_workchunk(workchunk *fc);
		static MasterControl 	*master_control;
		//static WorkTaskManager	*work_tsk_mgr;
	private:
};



#endif


