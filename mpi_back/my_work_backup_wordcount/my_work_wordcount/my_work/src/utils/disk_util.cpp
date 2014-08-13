#include <sys/statfs.h>  
#include <stdio.h>  
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

#include "disk_util.h"

int IsDir(char *filename)
{
	if(filename == NULL)
	{
		return 0;
	}

	if(opendir(filename) != NULL)
	{
		printf("%s is dir\r\n", filename);
		return 1;
	}
	else
	{
		printf("%s is file\r\n", filename);
		return 0;
	}

	return 0;
}

int freefileList(fileList *filelist)
{
	fileList *next = NULL;
	fileList *iter = NULL;

	if(filelist == NULL)
	{
		printf("filelist == NULL, can not freefileList.\r\n");
		return -1;
	}

	iter = filelist;
	while(iter != NULL)
	{
		next = iter->nextfile;
		free(iter);
		iter = next;
	}

}

int getfileSize(char *fileName)
{
	struct stat buf;

	if(fileName == NULL)
	{
		printf("fileName == NULL, can not get filesize.\r\n");
		return -1;
	}

	if(stat(fileName, &buf)<0)
	{
		return -1;
	}

	return (unsigned long)buf.st_size;
}

int removeFile(char *fileName)
{
	struct stat buf;

	if(fileName == NULL)
	{
		printf("fileName == NULL, can not remove.\r\n");
		return -1;
	}

	unlink(fileName);


	return 0;
}

fileList * getFileList(char *name)
{
	struct dirent *dir_env	= NULL;
	struct stat stat_file;
	char name_temp[1024];
	fileList *head	= NULL;
	fileList *cur	= NULL;
	DIR * dir;


	head =(fileList*)malloc(sizeof(fileList));


	head->nextfile = NULL;

	dir=opendir(name);

	if(dir == NULL)
	{
		printf("opendir %s failed.\r\n", name);
		return NULL;
	}

	while(dir_env = readdir(dir))
	{

		if((strcmp(dir_env->d_name,".") == 0) || (strcmp(dir_env->d_name,"..") == 0))
			continue;

		strcpy(name_temp, name);
		strcat(name_temp, "/");
		strcat(name_temp, dir_env->d_name);

		stat(name_temp,&stat_file);
		cur=(fileList*)malloc(sizeof(fileList));

		strcpy(cur->file.name,dir_env->d_name);
		strcpy(cur->file.fullName,name_temp);

		if( S_ISDIR(stat_file.st_mode))
		{
			continue; //no need subdir
			cur->file.size = 0;
			strcpy(cur->file.type,"dir");
			strcat(cur->file.fullName,"/");
		}else
		{
			cur->file.size = stat_file.st_size;
			strcpy(cur->file.type,"file");
		}

		cur->file.mod_time = stat_file.st_mtime;
		cur->file.size	   = getfileSize(cur->file.fullName);

		if(head->nextfile ==NULL)
		{
			head->nextfile = cur;
			cur->nextfile = NULL;
			printf("1file is %s\r\n", cur->file.fullName);
		}else
		{
			cur->nextfile = head->nextfile;
			head->nextfile = cur;
			printf("2file is %s\r\n", cur->file.fullName);
		}
	}


	return head->nextfile;
}

int getRemainSize(char *disk_partition) //return xxx Mbyte
{  
	struct statfs diskInfo;  
	unsigned long long blocksize;
	unsigned long long totalsize;
	unsigned long long freeDisk;
	unsigned long long availableDisk;

	int ret;

	if(disk_partition == NULL)
	{
		printf("disk_partition == NULL\r\n");
		return 0;
	}


	ret = statfs(disk_partition, &diskInfo);  
	if(ret != 0)
	{
		printf("statfs %s failed.\r\n", disk_partition);
		return 0;
	}

	blocksize = diskInfo.f_bsize;    

	totalsize = blocksize * diskInfo.f_blocks;
	freeDisk = diskInfo.f_bfree * blocksize; 
	availableDisk = diskInfo.f_bavail * blocksize;  

	printf("TotalSize = %llu MB Disk_free = %llu KB = %llu MB\nDisk_available = %llu KB = %llu MB\n",   
			totalsize>>20, freeDisk>>10, freeDisk>>20, availableDisk>>10, availableDisk>>20);  

	return 0;  
}  


int printfiles(char *dir)
{
	fileList *list = NULL;
	fileList *iter = NULL;

	if(dir == NULL)
	{
		printf("dir == NULL, can not printfiles.\r\n");	
		return -1;
	}

	list = getFileList(dir);

	if(list != NULL)
	{
		iter = list;
		while(iter != NULL)
		{
			printf("--------------------------------------------------\r\n");
			printf("filename is %s\r\n", iter->file.name);
			printf("fullname is %s\r\n", iter->file.fullName);
			printf("type is %s\r\n", iter->file.type);
			printf("modtime is %lu\r\n", iter->file.mod_time);
			printf("size is %lu\r\n", iter->file.size);
			printf("size is %lu M\r\n", iter->file.size >> 20);

			iter = iter->nextfile;
		}

		freefileList(list);
	}

}
