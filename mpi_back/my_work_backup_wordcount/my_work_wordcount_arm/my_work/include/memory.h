/*========================================================================
#   FileName: memory.h
#     Author: wangjian
#      Email: kuangrenyx@qq.com
#    Version: 1.0.0
#   HomePage: http://www.xxxxxxxxx.cn
# LastChange: 2013-06-05 16:43:59
========================================================================*/
#ifndef __MEMORY_H__
#define __MEMORY_H__


#include <sys/types.h>


class Memory {
	public:
		Memory();
		~Memory();

		static void *mem_malloc(size_t size);
		static void *mem_calloc(size_t num, size_t size);
		static void *mem_realloc(void *ptr, size_t size);
		static void mem_memcpy(void *dest, void *src, size_t size);
		static void mem_memset(void *s, int c, int n);
		static void mem_free(void *ptr);
		static void mem_freeall();

	private:
};











#endif


