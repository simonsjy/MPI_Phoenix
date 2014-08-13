/*========================================================================
#   FileName: mapreduce.cpp
#     Author: wangjian
#      Email: kuangrenyx@qq.com
#    Version: 1.0.0
#   HomePage: http://www.xxxxxxxxx.cn
# LastChange: 2013-11-21 17:45:20
========================================================================*/
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>
#include "mapreduce.h"
#include "master_control.h"
#include "system_debug.h"
#include "memory.h"
#include "disk_util.h"


int MapReduce::myproc;


MapReduce::MapReduce()
{
	int flag;

	MPI_Initialized(&flag);

	if(!flag)
	{
		int argc = 0;
		char **argv = NULL;
		MPI_Init(&argc, &argv);
	}

	comm = MPI_COMM_WORLD;
	MPI_Comm_rank(comm, &myproc);
	MPI_Comm_size(comm, &nprocs);

	nfile = 0;
	files = NULL;

}

MapReduce::~MapReduce()
{

}

void MapReduce::schedule(MapReduce_ARG *mr_arg1)
{
	int startwork = 0;
	MapReduce_ARG *mr_arg = (MapReduce_ARG *)Memory::mem_malloc(sizeof(MapReduce_ARG));
	memcpy(mr_arg, mr_arg1, sizeof(MapReduce_ARG));

	if(myproc == 0)
	{
		int maxfile = 0;

		inputfile_arg 		*inputfile 	= &mr_arg->inputfile;
		master_control_arg 	*mca 		= &mr_arg->mca;

		SYS_ASSERT(inputfile != NULL);
		SYS_ASSERT(mca != NULL);

		mca->nfile = 0;
		mca->files = NULL;

		//if current inputfile is a directory, traverse all files in subdirectories.
		if((inputfile->nfile == 1) && IsDir(inputfile->files[0]))
		{
			printf("before sacnfiles...\r\n");
			scanfiles(inputfile->files[0], mca->nfile, maxfile, mca->files);

		}
		else
		{
			for (int i = 0; i < inputfile->nfile; i++)
				findfiles(inputfile->files[i], inputfile->recurse, inputfile->readflag, mca->nfile, maxfile, mca->files);
		}

		SYS_INFO("mca->nfiles is %d\r\n", mca->nfile);
		for(int i = 0; i < mca->nfile; i++)
		{
			SYS_INFO("file %d is %s", i, mca->files[i]);
		}

		if(mca->nfile > 0)
		{
			startwork = 1;
		}
	}

	MPI_Bcast(&startwork, 1, MPI_INT, 0, MPI_COMM_WORLD);

	if(startwork == 0)
	{
		SYS_INFO("no work to do, exit\r\n");
		return;
	}
	//bcastfiles(nfile,files);

#if 0
	if(myproc == 0)
	{
		MasterControl *mc = new MasterControl(&mr_arg->mca, comm);

		SYS_INFO("masternode start work...\r\n");
		mc->startwork();

		sleep(50);

		SYS_INFO("masternode execute over...\r\n");
	}
	else
	{
		WorkMapReduce *wmr = new WorkMapReduce(&mr_arg->work_ctlarg, comm);
		SYS_INFO("worknode %d start work...\r\n", myproc);
		wmr->schedule();
		sleep(50);
		SYS_INFO("All worknode execute over...\r\n");
	}
#endif

	
	MPI_Barrier(MPI_COMM_WORLD);
	
	{
		ranks[0] = 0;
		MPI_Comm_group(MPI_COMM_WORLD, &group);
		MPI_Group_excl(group, 1, ranks, &newgroup);
		MPI_Comm_create(MPI_COMM_WORLD, newgroup, &newcomm);
	}

	//Not all node do it, exclude proc 0
	if(myproc != 0)
	{
		WorkMapReduce *wmr = new WorkMapReduce(&mr_arg->work_ctlarg, newcomm);

		SYS_INFO("myproc %d start work...\r\n", myproc);
		wmr->startwork();
		SYS_INFO("myproc %d execute over...\r\n", myproc);
	}

	if(myproc == 0)
	{
		MasterControl *mc = new MasterControl(&mr_arg->mca, comm);

		SYS_INFO("masternode startwork...\r\n");
		mc->startwork();
		SYS_INFO("masternode execute over...\r\n");
	}

	printf("proc%d over execute...\r\n", myproc);
	MPI_Barrier(MPI_COMM_WORLD);

	SYS_INFO("All procs done...\r\n");
	//for (int i = 0; i < nfile; i++) Memory::mem_free(files[i]);
	//Memory::mem_free(files);
}

int MapReduce::scanfiles(char *str, int &nfile, int &maxfile, char **&files)
{
	fileList *list = NULL;
	fileList *iter = NULL;
	fileList *freeptr = NULL;

	int count = 0;

	if(str == NULL)
	{
		return -1;
	}


	list = getFileList(str);

	if(list == NULL)
	{
		printf("%s is empty, have no file to process.\r\n");	
		return -1;
	}

	if(list != NULL)
	{
		iter = list;
		while(iter != NULL)
		{
			//printf("--------------------------------------------------\r\n");
			//printf("In scanfiles filename is %s\r\n", iter->file.name);
			//printf("In scanfiles fullname is %s\r\n", iter->file.fullName);

			if (nfile == maxfile) 
			{
				maxfile += FILECHUNK;
				printf("In scanfiles mem_realloc size %d\r\n", maxfile * sizeof(char*));
				files = (char **) Memory::mem_realloc(files,maxfile*sizeof(char *));
			}
			int n = strlen(str) + 1;
			//printf("In scanfiles mem_malloc size %d count %d\r\n", n, count++);
			files[nfile] = (char*)Memory::mem_malloc(strlen(iter->file.fullName)+1);
			strcpy(files[nfile],iter->file.fullName);
			nfile++;
			
			freeptr = iter;
			iter = iter->nextfile;
		}

		freefileList(list);
	}

}


void MapReduce::findfiles(char *str, int recurse, int readflag,
		int &nfile, int &maxfile, char **&files)
{
	int err,n;
	struct stat buf;
	char newstr[MAX_NAME_LEN];

	//SYS_DEBUG("nfile0 is %d", nfile);
	SYS_ASSERT(str != NULL);

	err = stat(str,&buf);
	if (err) 
	{
		SYS_ERROR("Could not query status of file %s in map", str);
	}
	else if (S_ISREG(buf.st_mode)) addfiles(str,readflag,nfile,maxfile,files);
	else if (S_ISDIR(buf.st_mode)) 
	{
		struct dirent *ep;
		DIR *dp = opendir(str);
		if (dp == NULL) 
		{
			SYS_ERROR("Cannot open directory %s to search for files in map",str);
		}
		while (ep = readdir(dp)) 
		{
			if (ep->d_name[0] == '.') continue;
			sprintf(newstr,"%s/%s",str,ep->d_name);
			err = stat(newstr,&buf);
			if (S_ISREG(buf.st_mode)) addfiles(newstr,readflag,nfile,maxfile,files);
			else if (S_ISDIR(buf.st_mode) && recurse)
				findfiles(newstr,recurse,readflag,nfile,maxfile,files);
		}
		closedir(dp);
	} else {
		SYS_ERROR("Invalid filename %s in map",str);
	}
}


void MapReduce::addfiles(char *str, int readflag, 
		int &nfile, int &maxfile, char **&files)
{
	if (!readflag) 
	{
		if (nfile == maxfile) 
		{
			maxfile += FILECHUNK;
			files = (char **) Memory::mem_realloc(files,maxfile*sizeof(char *));
		}
		int n = strlen(str) + 1;
		files[nfile] = (char*)Memory::mem_malloc(n);
		strcpy(files[nfile],str);
		nfile++;
		return;
	}

	FILE *fp = fopen(str,"r");
	if (fp == NULL) 
	{
		SYS_ERROR("Could not open file %s of filenames in map",str);
	}

	char line[MAXLINE];

	while (fgets(line,MAXLINE,fp)) 
	{
		char *ptr = line;
		while (isspace(*ptr)) ptr++;
		if (strlen(ptr) == 0) 
		{
			SYS_ERROR("Blank line in file %s of filenames in map",str);
		}
		char *ptr2 = ptr + strlen(ptr) - 1;
		while (isspace(*ptr2)) ptr2--;
		ptr2++;
		*ptr2 = '\0';

		if (nfile == maxfile) 
		{
			maxfile += FILECHUNK;
			files = (char **) Memory::mem_realloc(files,maxfile*sizeof(char *));
		}

		int n = strlen(ptr) + 1;
		files[nfile] = new char[n];
		strcpy(files[nfile],ptr);
		nfile++;
		//SYS_DEBUG("nfile2 is %d", nfile);
	}

	fclose(fp);
}

void MapReduce::bcastfiles(int &nfile, char **&files)
{
	MPI_Bcast(&nfile,1,MPI_INT,0,comm);

	if (myproc > 0)
		files = (char **) Memory::mem_realloc(files,nfile*sizeof(char *));

	int n;
	for (int i = 0; i < nfile; i++) 
	{
		if (myproc == 0) n = strlen(files[i]) + 1;
		MPI_Bcast(&n,1,MPI_INT,0,comm);
		if (myproc > 0) files[i] = new char[n];
		MPI_Bcast(files[i],n,MPI_CHAR,0,comm);
	}
}

