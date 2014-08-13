#include "mapreduce.h"
#include "system_debug.h"
#include <stdio.h>

int main(int argc, char **argv)
{
	MPI_Init(&argc, &argv);
	MapReduce *mr = new MapReduce();

	int procid = MapReduce::getProcId();

	printf("procid is %d\r\n", procid);


	SYS_ASSERT(1 == 1);
	SYS_ASSERT(1 == 0);

	SYS_INFO("test SYS_INFO");
	SYS_ERROR("test SYS_ERROR");
	SYS_WARNING("test SYS_WARNING");
	SYS_ALERT("test SYS_ALERT");

	return 0;
}
