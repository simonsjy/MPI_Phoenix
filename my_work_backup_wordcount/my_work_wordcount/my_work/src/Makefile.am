INCLUDES=-I$(top_srcdir)/include -I$(top_srcdir)/include/worknode -I$(top_srcdir)/src/utils
AM_LDFLAGS = -L$(top_srcdir)/lib

AM_CXXFLAGS = -Wall -O3 -DMMAP_POPULATE -fstrict-aliasing -Wstrict-aliasing  -D__x86__  -D_LINUX_

noinst_LIBRARIES   = libmympi.a 


libmympi_a_SOURCES 	= memory.cpp  mapreduce.cpp master_control.cpp filetask_manager.cpp scheduler.cpp	\
					env_manager.cpp \
					worknode/work_mapreduce.cpp \
					worknode/processor.cpp \
					worknode/core/task_queue.cpp \
					worknode/core/thread_pool.cpp \
					utils/disk_util.cpp
