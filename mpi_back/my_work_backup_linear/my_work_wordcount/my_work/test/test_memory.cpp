#include "system_debug.h"
#include "mapreduce.h"
#include "memory.h"
#include <stdio.h>

int main()
{
	MapReduce *mr = new MapReduce();

	int procid = MapReduce::getProcId();

	printf("procid is %d\r\n", procid);

	void *ptr = NULL;

	//ptr = Memory::mem_malloc(-1);
	//ptr = Memory::mem_realloc(ptr, 200);
	//Memory::mem_free(ptr);
	ptr = Memory::mem_malloc(100);
	printf("ptr is %p\r\n", ptr);
	ptr = Memory::mem_realloc(ptr, 200*1024);
	printf("ptr is %p\r\n", ptr);
	Memory::mem_free(ptr);

	return 0;
}
