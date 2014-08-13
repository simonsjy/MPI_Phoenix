#ifndef __DISK_UTIL_H__
#define __DISK_UTIL_H__


#ifdef __cplusplus 
extern "C" {
#endif

typedef struct fileInfo{
	char name[256];
	char fullName[1024];
	char type[10];
	time_t mod_time;
	unsigned long  size;
}fileInfo;

typedef struct filelist
{
	fileInfo file;
	struct filelist* nextfile;
}fileList;


int IsDir(char *filename);
fileList * getFileList(char *name);
int freefileList(fileList *filelist);


int getfileSize(char *fileName);
int getRemainSize(char *disk_partition);
int freeDiskSpace(char *dirname);



#ifdef __cplusplus 
}
#endif





#endif

