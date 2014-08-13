/*========================================================================
#   FileName: processor.h
#     Author: wangjian
#      Email: kuangrenyx@qq.com
#    Version: 1.0.0
#   HomePage: http://www.xxxxxxxxx.cn
# LastChange: 2013-06-08 11:37:18
========================================================================*/
#ifndef __PROCESSOR_H__
#define __PROCESSOR_H__


int 	proc_get_num_cpus (void);
int 	proc_bind_thread (int cpu_id);
int 	proc_unbind_thread ();
int 	proc_is_available (int cpu_id);
int 	proc_get_cpuid (void);

#endif
