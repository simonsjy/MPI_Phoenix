/*========================================================================
#   FileName: env_manager.cpp
#     Author: wangjian
#      Email: kuangrenyx@qq.com
#    Version: 1.0.0
#   HomePage: http://www.xxxxxxxxx.cn
# LastChange: 2013-06-10 16:42:56
========================================================================*/
#include "env_manager.h"


MasterControl 	*EnvManager::master_control;
//WorkTaskManager	*EnvManager::work_tsk_mgr;

EnvManager::EnvManager()
{

}

EnvManager::~EnvManager()
{

}

void EnvManager::add_filechunk(filechunk *fc)
{
	master_control->addtask(fc);
}

#if 0
void EnvManager::add_workchunk(workchunk *wc)
{
	//work_tsk_mgr->addtask(wc);
}
#endif
