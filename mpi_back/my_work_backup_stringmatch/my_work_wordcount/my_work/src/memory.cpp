/*========================================================================
#   FileName: memory.cpp
#     Author: wangjian
#      Email: kuangrenyx@qq.com
#    Version: 1.0.0
#   HomePage: http://www.xxxxxxxxx.cn
# LastChange: 2013-06-11 12:08:56
========================================================================*/
#include "system_debug.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


Memory::Memory()
{

}

Memory::~Memory() 
{

}

void *Memory::mem_malloc(size_t size) 
{
	void *ptr = NULL;

	if(size <= 0)
	{
		SYS_DEBUG("mem_malloc size parameter must > 0");
		return NULL;
	}

	//SYS_INFO("malloc %d before...\r\n", size);
	ptr = malloc(size);
	//SYS_INFO("malloc %d after...\r\n", size);
	SYS_ASSERT(ptr != NULL);

	return ptr;
}

void *Memory::mem_calloc(size_t num, size_t size) 
{
	void *ptr = NULL;

	if(size <= 0)
	{
		SYS_DEBUG("mem_malloc size parameter must > 0");
		return NULL;
	}

	ptr = calloc(num, size);
	SYS_ASSERT(ptr != NULL);

	return ptr;

}

void *Memory::mem_realloc(void *ptr, size_t size)
{
	if(size <= 0)
	{
		if(ptr != NULL)
			mem_free(ptr);
		return NULL;
	}

	ptr = realloc(ptr, size);
	SYS_ASSERT(ptr != NULL);

	return ptr;
}



void Memory::mem_memcpy(void *dest, void *src, size_t size)
{
	if(src == NULL || dest == NULL)
	{
		SYS_DEBUG("mem_memcpy src or dest must not be NULL");
		return;
	}

	memcpy(dest, src, size);
	return;
}

void Memory::mem_memset(void *s, int c, int n)
{
	if(s == NULL)
	{
		SYS_DEBUG("mem_memset buffer ptr must not be NULL");
		return;
	}

	memset(s, c, n);
}

void Memory::mem_free(void *ptr)
{
	if(ptr == NULL) return;

	free(ptr);
	return;
}

void mem_freeall()
{
}











