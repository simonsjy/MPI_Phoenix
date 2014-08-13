/*========================================================================
#   FileName: test_wordcount.cpp
#     Author: wangjian
#      Email: kuangrenyx@qq.com
#    Version: 1.0.0
#   HomePage: http://www.xxxxxxxxx.cn
# LastChange: 2013-10-31 16:42:15
========================================================================*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ctype.h>
#include <mpi.h>
#include "mapreduce.h"
#include "system_debug.h"
#include "filechunk.h"
#include "master_control.h"
#include "env_manager.h"
#include "memory.h"

enum {
    IN_WORD,
    NOT_IN_WORD
};

#if 0
void custom_split_file(char *filename)
{
	filechunk chunk;
	//SYS_INFO("In custom_splitfile...name:%s",filename);
	
	chunk.offset = 0;

	SYS_INFO("open filename:%s",filename);
	int fd = open(filename, O_RDONLY);
	struct stat finfo;
	assert(fd >= 0);
	fstat(fd, &finfo);
	chunk.chunksize = finfo.st_size;
	strncpy(chunk.filename, filename, strlen(filename)+1);

	EnvManager::add_filechunk(&chunk);
}
#endif

void custom_split_file(char *filename)
{
	filechunk chunk;
	FILE *fp = NULL;
	int filelen;
	int chunksize;
	int i;
	//SYS_INFO("In custom_splitfile...name:%s",filename);
	
	int numsplits = 4;
	int lastoffset = 0;

	int nproc;

	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	printf("nproc is %d\r\n", nproc-1);

	numsplits = nproc-1;

	SYS_INFO("open filename:%s",filename);
	fp = fopen(filename, "rb");
	assert(fp != NULL);

	fseek(fp, 0, SEEK_END);
	filelen = ftell(fp);

	chunksize = filelen / numsplits;

	strncpy(chunk.filename, filename, strlen(filename)+1);


	printf("filelen is %d\r\n", filelen);

	for(i = 0;  i < numsplits; i++)
	{
		chunk.offset = lastoffset;
		if(i == numsplits - 1)
		{
			chunk.chunksize = filelen - chunk.offset;
		}
		else
		{
			chunk.chunksize = chunksize;
		}
		lastoffset += chunk.chunksize;
		printf("split %d offset %d chunksize %d\r\n", i, chunk.offset, chunk.chunksize);
		EnvManager::add_filechunk(&chunk);
	}
	//chunk.chunksize = filelen;


#if 0
	chunk.offset = chunksize;
	//chunk.offset = 0;
	chunk.chunksize = chunksize;
	//chunk.chunksize = filelen;

	EnvManager::add_filechunk(&chunk);

	chunk.offset = chunksize*2;
	//chunk.offset = 0;
	chunk.chunksize = filelen - chunksize*2;
	//chunk.chunksize = filelen;

	EnvManager::add_filechunk(&chunk);
#endif

	fclose(fp);

#if 0
	int fd = open(filename, O_RDONLY);
	struct stat finfo;
	assert(fd >= 0);
	fstat(fd, &finfo);
	chunk.chunksize = finfo.st_size;
	strncpy(chunk.filename, filename, strlen(filename)+1);

	EnvManager::add_filechunk(&chunk);
#endif
}


int do_work(filechunk *fc, MPI_Comm newcomm);

int main(int argc, char **argv)
{
	MPI_Init(&argc, &argv);
	int me, nproc;
	char machine[256];

	MPI_Comm_rank(MPI_COMM_WORLD, &me);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);

	//printf("me is %d.\r\n", me);
	//printf("nproc  is %d.\r\n", nproc);

	if(argc <= 1)
	{
		printf("syntax: test_word_count file1...\r\n");
		MPI_Abort(MPI_COMM_WORLD, 1);
		return -1;
	}

	
	if(gethostname(machine, 256) == 0)
	{
		SYS_INFO("proc %d is %s\n", me, machine);
	}

	MapReduce *mr = new MapReduce();

	MapReduce_ARG mr_arg;
	mr_arg.inputfile.nfile	= argc - 1;
	mr_arg.inputfile.files 	= &argv[1];
	mr_arg.inputfile.recurse 	= 1;
	mr_arg.inputfile.readflag = 1;

	mr_arg.mca.split_file_callback = custom_split_file;

	mr_arg.work_ctlarg.do_work_func = do_work;

	mr->schedule(&mr_arg);

	SYS_INFO("I am proc %d", MapReduce::getProcId());

	SYS_INFO("application exec over1 me %d machine %s...\r\n", me, machine);

	MPI_Finalize();

	SYS_INFO("application exec over2...\r\n");

	return 0;
}
