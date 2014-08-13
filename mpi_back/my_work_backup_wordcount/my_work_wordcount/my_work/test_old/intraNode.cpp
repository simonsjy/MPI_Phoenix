/*========================================================================
#   FileName: debug.cpp
#     Author: wangjian
#      Email: kuangrenyx@qq.com
#    Version: 1.0.0
#   HomePage: http://www.xxxxxxxxx.cn
# LastChange: 2013-10-24 15:49:24
========================================================================*/
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>

#ifdef TBB
#include "tbb/scalable_allocator.h"
#endif

#include "map_reduce.h"
#include "filechunk.h"

#define DEFAULT_DISP_NUM 10

// a passage from the text. The input data to the Map-Reduce
struct wc_string {
    char* data;
    uint64_t len;
};

	// a single null-terminated word
struct wc_word {
    char* data;
    
    // necessary functions to use this as a key
    bool operator<(wc_word const& other) const {
        return strcmp(data, other.data) < 0;
    }
    bool operator==(wc_word const& other) const {
        return strcmp(data, other.data) == 0;
    }
};


// a hash for the word
struct wc_word_hash
{
    // FNV-1a hash for 64 bits
    size_t operator()(wc_word const& key) const
    {
        char* h = key.data;
        uint64_t v = 14695981039346656037ULL;
        while (*h != 0)
            v = (v ^ (size_t)(*(h++))) * 1099511628211ULL;
        return v;
    }
};

#ifdef MUST_USE_FIXED_HASH
class WordsMR : public MapReduceSort<WordsMR, wc_string, wc_word, uint64_t, fixed_hash_container<wc_word, uint64_t, sum_combiner, 32768, wc_word_hash
#else
class WordsMR : public MapReduceSort<WordsMR, wc_string, wc_word, uint64_t, hash_container<wc_word, uint64_t, sum_combiner, wc_word_hash 
#endif
#ifdef TBB
    , tbb::scalable_allocator
#endif
> >
{
    char* data;
    uint64_t data_size;
    uint64_t chunk_size;
    uint64_t splitter_pos;
public:

    explicit WordsMR(char* _data, uint64_t length, uint64_t _chunk_size, MPI_Comm comm) :
        data(_data), data_size(length), chunk_size(_chunk_size), 
            splitter_pos(0) {
				newcomm = comm;
				run_type = RUN_MRM_DISP;
				}

    void* locate(data_type* str, uint64_t len) const
    {
        return str->data;
    }

	char *getkey(key_type &key, int &len)
	{
		len = strlen(key.data)+1;
		return key.data;
	}

	char *getvalue(value_type &value, int &len)
	{
		len = sizeof(value_type);

#if 0
		printf("In getvalue value %d\r\n", value);
		printf("In getvalue value size %d\r\n", sizeof(value_type));
		printf("\r\n\r\n");
#endif

		return (char*)&value;
	}

    void map2(uint64_t data, uint64_t len, map_container& out) const 
	{
		static int debug = 0;
		int index = 0;
		char *data_ptr = (char*)data;
		int *keysize_ptr;
		int *valuesize_ptr;
		char *key_ptr;
		int value;


		//printf("In User map2 debug %d data %llx len %llu\r\n", debug++, data, len );

#if 1
		for(data_ptr = data_ptr+index; (uint64_t)data_ptr < (uint64_t)(data+len);)
		{
			keysize_ptr = (int*)data_ptr;
			valuesize_ptr = (int*)(data_ptr + sizeof(int));
				
			//dprintf("dataptr %llu dataptr_len %llu\r\n", (uint64_t)data_ptr, (uint64_t)(data_ptr+len));
			//dprintf("In Usermap2 keysize %llu valuesize %llu debug %d\r\n", *keysize_ptr, *valuesize_ptr, debug++);
			//dprintf("In Usermap2 key %s value %llu\r\n",(char*)((char*)keysize_ptr + 2*sizeof(uint64_t)), *(uint64_t*)((char*)keysize_ptr + 2*sizeof(uint64_t) + *keysize_ptr));

			data_ptr = data_ptr + 2*sizeof(int) + *keysize_ptr + *valuesize_ptr;

			key_ptr = (char*)keysize_ptr + 2*sizeof(int);
			value = *(int*)((char*)keysize_ptr + 2*sizeof(int) + *keysize_ptr);
			wc_word word = { key_ptr };
			emit_intermediate(out, word, value);

		}
#endif
		
		//printf("Out User map2 debug %d...........\r\n\r\n", debug++);
	}

    void map(data_type const& s, map_container& out) const
    {
        for (uint64_t i = 0; i < s.len; i++)
        {
            s.data[i] = toupper(s.data[i]);
        }

        uint64_t i = 0;
        while(i < s.len)
        {            
            while(i < s.len && (s.data[i] < 'A' || s.data[i] > 'Z'))
                i++;
            uint64_t start = i;
            while(i < s.len && ((s.data[i] >= 'A' && s.data[i] <= 'Z') || s.data[i] == '\''))
                i++;
            if(i > start)
            {
                s.data[i] = 0;
                wc_word word = { s.data+start };
                emit_intermediate(out, word, 1);
            }
        }
    }


	void map2(char *data, int datanum, map_container& out) const
    {

		//emit_intermediate(out, data, 1);
    }


    /** wordcount split()
     *  Memory map the file and divide file on a word border i.e. a space.
     */
    int split(wc_string& out)
    {
        /* End of data reached, return FALSE. */
        if ((uint64_t)splitter_pos >= data_size)
        {
            return 0;
        }

        /* Determine the nominal end point. */
        uint64_t end = std::min(splitter_pos + chunk_size, data_size);

        /* Move end point to next word break */
        while(end < data_size && 
            data[end] != ' ' && data[end] != '\t' &&
            data[end] != '\r' && data[end] != '\n')
            end++;

        /* Set the start of the next data. */
        out.data = data + splitter_pos;
        out.len = end - splitter_pos;
        
        splitter_pos = end;

        /* Return true since the out data is valid. */
        return 1;
    }

    bool sort(keyval const& a, keyval const& b) const
    {
        return a.val < b.val || (a.val == b.val && strcmp(a.key.data, b.key.data) > 0);
    }
};

#define NO_MMAP

int do_work(filechunk *fc, MPI_Comm newcomm)
{
    int fd;
	FILE *fp = NULL;
    char * fdata;
    unsigned int disp_num;
    struct stat finfo;
    char * fname, * disp_num_str;
    struct timespec begin, end;

	int offset;
	int chunksize;
	int alreadyread;

	offset = fc->offset;
	chunksize = fc->chunksize;

    get_time (begin);

    // Make sure a filename is specified
	
	printf("do_work filename is %s\r\n", fc->filename);
    fname = fc->filename;
    disp_num_str = "10";

    printf("Wordcount: Running...\n");

    // Read in the file
    CHECK_ERROR((fd = open(fname, O_RDONLY)) < 0);
    // Get the file info (for file length)
    CHECK_ERROR(fstat(fd, &finfo) < 0);
#ifndef NO_MMAP
#ifdef MMAP_POPULATE
    // Memory map the file
    CHECK_ERROR((fdata = (char*)mmap(0, finfo.st_size + 1, 
        PROT_READ, MAP_PRIVATE | MAP_POPULATE, fd, 0)) == NULL);
#else
    // Memory map the file
    CHECK_ERROR((fdata = (char*)mmap(0, finfo.st_size + 1, 
        PROT_READ, MAP_PRIVATE, fd, 0)) == NULL);
#endif
#else
    uint64_t r = 0;

#if 0
    fdata = (char *)malloc (finfo.st_size);
    CHECK_ERROR (fdata == NULL);
    while(r < (uint64_t)finfo.st_size)
        r += pread (fd, fdata + r, finfo.st_size, r);
    CHECK_ERROR (r != (uint64_t)finfo.st_size);
#else

	int myprocid;
	char machine[128];
	MPI_Comm_rank(MPI_COMM_WORLD, &myprocid);
	printf("myprocid %d open fname is %s\r\n",myprocid, fname);
	fp = fopen(fname, "rb");
	fdata = (char*)malloc(chunksize);

	if(gethostname(machine, 256) == 0)
	{
		printf("proc %d is %s\n", myprocid, machine);
	}
	assert(fdata != NULL);
	
	fseek(fp, offset, SEEK_SET);
	printf("--------------- before read over...\r\n");

	while(alreadyread < chunksize)
	{
		alreadyread += fread(fdata+alreadyread, 1, chunksize - alreadyread, fp);
	}

	//sleep(5);
	printf("read over...\r\n");

#endif

#endif    
    
    // Get the number of results to display
    CHECK_ERROR((disp_num = (disp_num_str == NULL) ? 
      DEFAULT_DISP_NUM : atoi(disp_num_str)) <= 0);

    get_time (end);

#ifdef TIMING
    print_time("initialize", begin, end);
#endif

    printf("Wordcount: Calling MapReduce Scheduler Wordcount\n");
    get_time (begin);
    std::vector<WordsMR::keyval> result;    
    //WordsMR mapReduce(fdata, finfo.st_size, 1024*1024);
	//
    WordsMR mapReduce(fdata, chunksize, 1024*1024, newcomm);
    CHECK_ERROR( mapReduce.run(result) < 0);
    get_time (end);

#ifdef TIMING
    print_time("library", begin, end);
#endif
    printf("Wordcount: MapReduce Completed\n");

    get_time (begin);

    unsigned int dn = std::min(disp_num, (unsigned int)result.size());
    printf("\nWordcount: Results (TOP %d of %lu):\n", dn, result.size());
    uint64_t total = 0;
    for (size_t i = 0; i < dn; i++)
    {
        //printf("%15s - %lu\n", result[result.size()-1-i].key.data, result[result.size()-1-i].val);
    }

    for(size_t i = 0; i < result.size(); i++)
    {
        total += result[i].val;
    }

    printf("Total: %lu\n", total);

#ifndef NO_MMAP
    CHECK_ERROR(munmap(fdata, finfo.st_size + 1) < 0);
#else
    free (fdata);
#endif
    CHECK_ERROR(close(fd) < 0);

    get_time (end);

#ifdef TIMING
    print_time("finalize", begin, end);
#endif

    return 0;
}

// vim: ts=8 sw=4 sts=4 smarttab smartindent
