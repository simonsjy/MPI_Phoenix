#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "cmem.h"
#include "bmp_header.h"
#include "call_dsp.h"

int libfog_process_dsp(char *datain, char *dataout)
{
	static int first = 1;
	static char *data, *jpeg;
	int len;
	short x, y, w, h;

	struct timeval start_time;
	struct timeval end_time;
	bitmapfile bmphead1;
	bitmapinfo bmpinfo1;


	CMEM_AllocParams cmem_params;

	//if(first == 1)
	//{
		CMEM_init();

		cmem_params.type = CMEM_POOL;
		cmem_params.flags = CMEM_NONCACHED;
		cmem_params.alignment = 32;


		data = (char*)CMEM_alloc(704*576*3, &cmem_params); 
		jpeg = (char*)CMEM_alloc(704*576*3, &cmem_params);

		if(data == NULL || jpeg == NULL)
		{
			printf("CMEM_alloc error.\r\n");
			return -1;
		}
		else
		{
			printf("CMEM_alloc success.\r\n");
			printf("data virtaddr is %x phys addr is %x.\r\n",data,CMEM_getPhys(data));
			printf("jpeg virtaddr is %x phys_addr is %x.\r\n",jpeg,CMEM_getPhys(jpeg));
		}

		RFOG_INIT(data,jpeg);
		first = 0;
	//}


	memcpy(&bmphead1, datain, 14);
	memcpy(&bmpinfo1, datain+14, 40);

	printf("head bfSize %d.\r\n", bmphead1.bfSize);
	printf("info biWidth %d.\r\n", bmpinfo1.biWidth);
	printf("info biHeight %d.\r\n", bmpinfo1.biHeight);
	printf("info biBitCount %d.\r\n", bmpinfo1.biBitCount);
	printf("info biSizeImage %d.\r\n", bmpinfo1.biSizeImage);

	memcpy(data, datain+54, bmpinfo1.biSizeImage);

	*((short *)data)     = 0;
	*((short *)(data+2)) = 0;
	*((short *)(data+4)) = bmpinfo1.biWidth;
	*((short *)(data+6)) = bmpinfo1.biHeight;

	printf("before encode....\r\n");
	gettimeofday(&start_time, NULL);
	RFOG_PROCESS();
	gettimeofday(&end_time, NULL);

	printf("consume time is %d ms.\r\n", (end_time.tv_usec / 1000 + end_time.tv_sec * 1000) - (start_time.tv_usec / 1000 + start_time.tv_sec * 1000)); 


	memcpy(dataout, datain, 54);
	memcpy(dataout+54, jpeg, bmpinfo1.biSizeImage);


	CMEM_free(data, &cmem_params);
	CMEM_free(jpeg, &cmem_params);
	RFOG_DESTROY();

	return 0;
}

