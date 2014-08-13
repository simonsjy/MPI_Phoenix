#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
int main(void){
	char rdbuf[30];
	memset(rdbuf,0,sizeof(rdbuf));
	int fd = open("/dev/ttyUSB2",O_RDWR|O_NDELAY);
	int fd2 = open("/dev/fd/1",O_RDONLY);
	if(fd<0||fd2<0)
	{
	exit(1);
	}
	else
	{	
	//	printf("open ttyusb2.\n");
		char *buf = "at+csqlvl";
		//write(fd,"at",2);
		int wn = write(fd,buf,strlen(buf));
		printf("write length:%d,error:%d\n",wn,errno);
		int rn = read(fd2,rdbuf,30);
		printf("read length:%d\n,error:%d\n",rn,errno);
		
		int i = 0;
		printf("rdbuf:\n");
		for(;i<30;i++){
		printf("%2x ",rdbuf[i]);
		}
		printf("\n");
		printf("rdbuf:%s\n",rdbuf);
	}
	return 0;
}
