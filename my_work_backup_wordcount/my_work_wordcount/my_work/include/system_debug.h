/*========================================================================
#   FileName: system_debug.h
#     Author: wangjian
#      Email: kuangrenyx@qq.com
#    Version: 1.0.0
#   HomePage: http://www.xxxxxxxxx.cn
# LastChange: 2013-06-05 16:34:58
========================================================================*/
#ifndef	__SYSTEM_DEBUG_H__ 
#define	__SYSTEM_DEBUG_H__

#include <stdio.h>
#include <assert.h>
#include "mapreduce.h"


#define SYS_INFO_ON         1
#define SYS_INFO_OFF        0

#define SYS_INFO_FLAG     	SYS_INFO_ON
#define SYS_ERROR_FLAG      SYS_INFO_ON
#define SYS_WARING_FLAG     SYS_INFO_ON
#define SYS_ALERT_FLAG     	SYS_INFO_ON
#define SYS_DEBUG_FLAG      SYS_INFO_ON

#define SYS_STDIN       stdin
#define SYS_STDOUT      stdout
#define SYS_STDERR      stderr

#ifdef __cplusplus
extern "C"{
#endif

#define SYS_ASSERT_ERROR(file_name, line_no, func_name ) do \
{ \
    fprintf(SYS_STDERR, "[Proc %d] [Assert failed]: [%s : %u] In func: %s\n", \
		MapReduce::getProcId(), file_name, line_no, func_name ); \
	assert(0);\
} while( 0 )

#define SYS_ASSERT( condition ) do \
{ \
    if( !(condition) ) \
    { \
	SYS_ASSERT_ERROR(  __FILE__, __LINE__, __PRETTY_FUNCTION__); \
    } \
} while( 0 )


#define SYS_INFO( format, args... ) \
{ \
    if( 1 == SYS_INFO_FLAG ) \
    { \
	fprintf( SYS_STDERR, "[Proc %d] [Info]: ", MapReduce::getProcId()); \
	fprintf( SYS_STDERR, "[%s : %u]  ", __FILE__, __LINE__ ); \
	fprintf( SYS_STDERR, format, ##args ); \
	fprintf( SYS_STDERR, "\n" ); \
    } \
}

#define SYS_ERROR( format, args... ) do \
{ \
    if( 1 == SYS_ERROR_FLAG ) \
    { \
	fprintf( SYS_STDERR, "[Proc %d] [Error]: ", MapReduce::getProcId() ); \
	fprintf( SYS_STDERR, "[%s : %u]  ", __FILE__, __LINE__ ); \
	fprintf( SYS_STDERR, format, ##args ); \
	fprintf( SYS_STDERR, "\n" ); \
    } \
} while( 0 )

#define SYS_WARNING( format, args... ) do \
{ \
    if( 1 == SYS_WARING_FLAG ) \
    { \
	fprintf( SYS_STDERR, "[Proc %d] [Waring]: ", MapReduce::getProcId() ); \
	fprintf( SYS_STDERR, "[%s : %u]  ", __FILE__, __LINE__ ); \
	fprintf( SYS_STDERR, format, ##args ); \
	fprintf( SYS_STDERR, "\n" ); \
    } \
} while( 0 )

#define SYS_ALERT( format, args... ) do \
{ \
    if( 1 == SYS_ALERT_FLAG ) \
    { \
	fprintf( SYS_STDERR, "[Proc %d] [Alert]: ", MapReduce::getProcId() ); \
	fprintf( SYS_STDERR, "[%s : %u]  ", __FILE__, __LINE__ ); \
	fprintf( SYS_STDERR, format, ##args ); \
	fprintf( SYS_STDERR, "\n" ); \
    } \
} while( 0 )

#define SYS_DEBUG( format, args... ) \
{ \
    if( 1 == SYS_DEBUG_FLAG ) \
    { \
	fprintf( SYS_STDERR, "[Proc %d] [Debug]: ", MapReduce::getProcId()); \
	fprintf( SYS_STDERR, "[%s : %u]  ", __FILE__, __LINE__ ); \
	fprintf( SYS_STDERR, format, ##args ); \
	fprintf( SYS_STDERR, "\n" ); \
    } \
}


#ifdef __cplusplus
}
#endif

#endif   


