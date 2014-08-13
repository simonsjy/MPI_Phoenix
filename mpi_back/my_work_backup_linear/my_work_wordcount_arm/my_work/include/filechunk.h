/*========================================================================
#   FileName: filechunk.h
#     Author: wangjian
#      Email: kuangrenyx@qq.com
#    Version: 1.0.0
#   HomePage: http://www.xxxxxxxxx.cn
# LastChange: 2013-10-18 14:06:32
========================================================================*/
#ifndef __FILECHUNK_H__
#define __FILECHUNK_H__


#define MAX_NAME_LEN 128

typedef struct filechunk_t {
	int 	issplit;	//0:file was splitted, 1:a filechunk is a whole file
	int 	nsplit;
	
	char 	filename[MAX_NAME_LEN];
	int 	offset;
	int 	chunksize;

	int 	nowork; //for proc exit

}filechunk;



#endif


