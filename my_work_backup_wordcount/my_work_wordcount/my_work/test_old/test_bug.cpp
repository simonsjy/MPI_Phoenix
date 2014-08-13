#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

//typedef long long uint64_t;

int main()
{
#if 0
	int filelen;
	char *rcv_data_buffer;
	FILE *fp = fopen("test.data", "rb");
	assert(fp != NULL);

	fseek(fp, 0, SEEK_END);
	filelen = ftell(fp);

	rewind(fp);

	rcv_data_buffer = (char*)malloc(filelen);
	assert(rcv_data_buffer != NULL);

	fread(rcv_data_buffer, 1, filelen, fp);
	fclose(fp);

	printf("sizeof long long %d\r\n", sizeof(uint64_t));
	char *index = rcv_data_buffer;
	for(int i = 0; i < 30; i++)
	{
		printf("%x ", index[i]);
	}

	for(int i = 0; i < 100; i++)
	{
		uint64_t *key_index = (uint64_t*)index;
		uint64_t *value_index = (uint64_t*)(index + sizeof(uint64_t));
		char *key, *value;

		printf("\r\nproc %d keysize %llu\r\n",0, *key_index);
		printf("\r\nproc %d valuesize %llu\r\n",0, *value_index);

		key = index + 2*sizeof(uint64_t);
		value = key + *key_index;

		index = index + *key_index + *value_index + 2*sizeof(uint64_t);

		key[strlen(key)] = '\0';
		printf("\r\nkey %s\r\n", key);
		printf("\r\nvalue %llu\r\n", *(uint64_t*)value);
	}
#endif
	char abc[]="0x1234567890000000";
	uint64_t def = 0x123;
	int *kk;

	printf("%lu\r\n", *(int*)abc);
	printf("%llu\r\n", def);
	//def = *(uint64_t*)abc;
	kk = (int*)abc;
	printf("kk %x\r\n", kk);
	printf("*kk %llu\r\n", kk[0]);
	printf("%llu\r\n", *(uint64_t*)abc);
	return 0;

}

