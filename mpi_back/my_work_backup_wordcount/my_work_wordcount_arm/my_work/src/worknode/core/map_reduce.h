/*========================================================================
#   FileName: map_reduce.h
#     Author: wangjian
#      Email: kuangrenyx@qq.com
#    Version: 1.0.0
#   HomePage: http://www.xxxxxxxxx.cn
# LastChange: 2013-10-24 12:14:40
========================================================================*/

#ifndef MAP_REDUCE_H_
#define MAP_REDUCE_H_

#include <assert.h>
#include <algorithm>
#include <vector>
#include <queue>
#include <limits>
#include <cmath>


#include <mpi.h>

#include "stddefines.h"
#include "processor.h"
#include "scheduler.h"
#include "task_queue.h"
#include "combiner.h"
#include "container.h"
#include "locality.h"
#include "thread_pool.h"

enum RUN_TYPE {
	RUN_MAP_ONLY	= 0x10,
	RUN_MAP_REDUCE 	= 0x11,
	RUN_MR_MERGE 	= 0x12,
	RUN_MRM_DISP	= 0x13,
};

template<typename Impl, typename D, typename K, typename V, 
    class Container = hash_container<K, V, buffer_combiner> >
class MapReduce
{
public:

    /* Standard data types for the function arguments and results */
    typedef D data_type;
    typedef V value_type;
    typedef K key_type;
    typedef Container container_type;

    typedef typename container_type::input_type map_container;
    typedef typename container_type::output_type reduce_iterator; 
 
    struct keyval
    {
        key_type key;
        value_type val;
    };


protected:

    // Parameters.
    uint64_t num_threads;               // # of threads to run.
    uint64_t thread_offset;             // cores to skip when assigning threads.

	//for run_dispatch, used by MPI
	MPI_Comm comm;	 	
	int myproc;
	int nprocs;

	int run_type;

	//for sync some procs, not all procs
	MPI_Comm 	newcomm;

    thread_pool* threadPool;            // Thread pool.
    task_queue* taskQueue;              // Queues of tasks.

    container_type container; 
    std::vector<keyval>* final_vals;    // Array to send to merge task.    
    
    uint64_t num_map_tasks;
    uint64_t num_reduce_tasks;

    virtual void run_map(data_type* data, uint64_t len);
	virtual void run_map2 (char *data, uint64_t datanum);

    virtual void run_reduce();
    virtual void run_merge();

    virtual void run_dispatch(std::vector<keyval>& result);
    
    virtual void map_worker(
        thread_loc const& loc, double& time, double& user_time, int& tasks);
    virtual void reduce_worker(
        thread_loc const& loc, double& time, double& user_time, int& tasks);
    virtual void merge_worker(
        thread_loc const& loc, double& time, double& user_time, int& tasks);

	virtual void map_worker2(
        thread_loc const& loc, double& time, double& user_time, int& tasks);

    // Data passed to the callback functions.
    struct thread_arg_t
    {
        // in
        MapReduce* mr;
        // out
        double user_time;
        double time;        
        int tasks;
    };

    static void map_callback(void* arg, thread_loc const& loc) { 
        thread_arg_t* t = (thread_arg_t*)arg; 
        t->mr->map_worker(loc, t->time, t->user_time, t->tasks); 
    }
	static void map_callback2(void* arg, thread_loc const& loc) { 
        thread_arg_t* t = (thread_arg_t*)arg; 
        t->mr->map_worker2(loc, t->time, t->user_time, t->tasks); 
    }
    static void reduce_callback(void* arg, thread_loc const& loc) { 
        thread_arg_t* t = (thread_arg_t*)arg; 
        t->mr->reduce_worker(loc, t->time, t->user_time, t->tasks);
    }
    static void merge_callback(void* arg, thread_loc const& loc) { 
        thread_arg_t* t = (thread_arg_t*)arg; 
        t->mr->merge_worker(loc, t->time, t->user_time, t->tasks); 
    }
    void start_workers (void (*callback)(void*, thread_loc const&), 
        int num_threads, char const* stage);    
    
    // the default split function...
    int split(data_type &a) { return 0; }

    // the default map function...
    void map(data_type const& a, map_container& m) const {}
    void map2(uint64_t data, uint64_t len, map_container& m) const {}
    
    // the default reduce function...
    void reduce(key_type const& key, reduce_iterator const& values, 
        std::vector<keyval>& out) const {
        value_type val;
        while (values.next(val))
        {
            keyval kv = {key, val};
            out.push_back(kv);
        }
    }

	char *getkey(key_type &key, int &len)  //used by run_dispatch
	{
		len = 0;
		return NULL;
	}

	char *getvalue(value_type &value, int &len) //used by run_dispatch
	{
		len = 0;
		return NULL;
	}

    // the default locator function...
    void* locate(data_type* data, uint64_t) const {
        return (void*)data;
    }

	void mapreduce_barrier()
	{
		MPI_Barrier(static_cast<Impl const*>(this)->newcomm);
		//MPI_Barrier(newcomm);
	}

public:

    MapReduce() : threadPool(NULL), taskQueue(NULL) {
        // Determine the number of threads to use. 
        // First check for an environment variable, then use the 
        // number of processors
        int threads = atoi(GETENV("MR_NUMTHREADS"));
        setThreads(threads > 0 ? threads : proc_get_num_cpus(), 0);

		//for run_dispatch, used by MPI
		comm = MPI_COMM_WORLD;
		MPI_Comm_rank(comm, &myproc);
		MPI_Comm_size(comm, &nprocs);
	}

    virtual ~MapReduce() {
        if(this->threadPool != NULL) delete this->threadPool;
        if(this->taskQueue != NULL) delete this->taskQueue;
    }

    // override the default thread offset and thread count.
    MapReduce& setThreads(int num_threads, sched_policy const* policy = NULL) {
        this->num_threads = (num_threads > 0) ? num_threads : this->num_threads;
        
        if(this->threadPool != NULL) delete this->threadPool;
        if(this->taskQueue != NULL) delete this->taskQueue;

        // Create thread pool and task queue
        sched_policy_strand_fill default_policy(0);
        this->threadPool = new thread_pool(
            num_threads, policy == NULL ? &default_policy : policy);
        this->taskQueue = new task_queue(num_threads, num_threads);

        return *this;
    }
    
    /* The main MapReduce engine. This is the function called by the 
     * application. It is responsible for creating and scheduling all map 
     * and reduce tasks, and also organizes and maintains the data which is 
     * passed from application to map tasks, map tasks to reduce tasks, and 
     * reduce tasks back to the application. Results are stored in result. 
     * A return value less than zero represents an error. This function is 
     * not thread safe.
     */
    int run(data_type *data, uint64_t count, std::vector<keyval>& result);

	int run2 (char *data, uint64_t datanum, std::vector<keyval>& result);

    // This version assumes that the split function is provided.
    int run(std::vector<keyval>& result);

    void emit_intermediate(typename container_type::input_type& i, 
        key_type const& k, value_type const& v) const {
	i[k].add(v);
    }
};

template<typename Impl, typename D, typename K, typename V, class Container>
int MapReduce<Impl, D, K, V, Container>::
run (std::vector<keyval>& result)
{
    timespec begin;    
    std::vector<D> data;
    uint64_t count;
    D chunk;

    // Run splitter to generate chunks
    get_time (begin);
    while (static_cast<Impl const*>(this)->split(chunk))
    {
        data.push_back(chunk);
    }
    count = data.size();
    print_time_elapsed("split phase", begin);

    return run(&data[0], count, result);
}

template<typename Impl, typename D, typename K, typename V, class Container>
int MapReduce<Impl, D, K, V, Container>::
run (D *data, uint64_t count, std::vector<keyval>& result)
{
    timespec begin;    
    timespec run_begin = get_time();
	int showline = 10;
	
	run_type = static_cast<Impl const*>(this)->run_type;
    // Initialize library
    get_time (begin);

    // Compute task counts (should make this more adjustable) and then 
    // allocate storage
	//dprintf("In run this->num_threads is %llu\r\n", this->num_threads);
    this->num_map_tasks = std::min(count, this->num_threads) * 16;
    this->num_reduce_tasks = this->num_threads;

    dprintf ("num_map_tasks = %d\n", num_map_tasks);
    dprintf ("num_reduce_tasks = %d\n", num_reduce_tasks);

    container.init(this->num_threads, this->num_reduce_tasks);
    this->final_vals = new std::vector<keyval>[this->num_threads];
    for(uint64_t i = 0; i < this->num_threads; i++) {
        // Try to avoid a reallocation. Very costly on Solaris.
        this->final_vals[i].reserve(100);
    }
    print_time_elapsed_proc(myproc, "library init", begin);

	if(run_type >= RUN_MAP_ONLY)
	{
		// Run map tasks and get intermediate values
		get_time (begin);
		run_map(&data[0], count);
		print_time_elapsed_proc(myproc, "map phase", begin);
	}

    //dprintf("In scheduler, all map tasks are done, now scheduling reduce tasks\n");

	if(run_type >= RUN_MAP_REDUCE)
	{
		// Run reduce tasks and get final values
		get_time (begin);
		run_reduce();
		print_time_elapsed_proc(myproc, "reduce phase", begin);
	}

    //dprintf("In scheduler, all reduce tasks are done, now scheduling merge tasks\n");

	if(run_type >= RUN_MR_MERGE)
	{
		get_time (begin);
		run_merge();
		print_time_elapsed_proc(myproc, "merge phase", begin);
		result.swap(*this->final_vals);
	}
    
    // Delete structures
    delete [] this->final_vals;
    

#if 0
	size_t keysizes = result.size();
	if(showline > keysizes)
	{
		showline = keysizes;
	}
	for(size_t i = 0; i < showline; i++)
	{
		dprintf("run proc %d %15s - %lu\n",myproc, result[result.size()-1-i].key.data, result[result.size()-1-i].val);
	}
	return 0;
#endif

	if(run_type >= RUN_MRM_DISP)
	{
		run_dispatch(result);
	}
    print_time_elapsed_proc(myproc, "run total time",  run_begin);
    return 0;
}

/**
 * Run map tasks and get intermediate values
 */
template<typename Impl, typename D, typename K, typename V, class Container>
void MapReduce<Impl, D, K, V, Container>::
run_map (data_type* data, uint64_t count)
{
    // Compute map task chunk size
    uint64_t chunk_size = 
        std::max(1, (int)ceil((double)count / this->num_map_tasks));
    
    // Generate tasks by splitting input data and add to queue.
    for(uint64_t i = 0; i < this->num_map_tasks; i++)
    {
        uint64_t start = chunk_size * i;

        if(start < count)
        {
            uint64_t len = std::min(chunk_size, count-start);
            int lgrp = loc_mem_to_lgrp (
                static_cast<Impl const*>(this)->locate(data+start, len));
                task_queue::task_t task = 
                    // For debugging, last element is normally padding
                    {    i, len, (uint64_t)(data+start), lgrp };    
            this->taskQueue->enqueue_seq (task, this->num_map_tasks, lgrp);
        }
    }

    start_workers (&map_callback, std::min(num_map_tasks, num_threads), "map"); 
}

template<typename Impl, typename D, typename K, typename V, class Container>
void MapReduce<Impl, D, K, V, Container>::
run_map2 (char *data, uint64_t datanum)
{
    // Compute map task chunk size
    uint64_t chunk_size = 
        std::max(1, (int)ceil((double)datanum / this->num_map_tasks));
    
	int *key_size;
	int *value_size;

	int data_index_s = 0;
	int data_index_e = 0;
	int count = 0;
	int debug_count = 0;

	dprintf("In run_map2 datanum %llu num_map_tasks %llu chunk_size %llu\r\n", datanum, num_map_tasks, chunk_size);

    // Generate tasks by splitting input data and add to queue.
    for(uint64_t i = 0; i < this->num_map_tasks; i++)
    {
		data_index_s = data_index_e;
		//dprintf("chunk_size %llu count %llu datanum %llu \r\n", chunk_size, count, datanum);
		for(uint64_t j = 0; (j < chunk_size) && (count < datanum); j++)
		{
			key_size = (int*)(data + data_index_e);
			value_size = (int*)(data + data_index_e + sizeof(int));

			data_index_e = data_index_e + 2*sizeof(int) + *key_size + *value_size;
			count++;

			//dprintf("proc %d what %s i %llu keysize %llu\r\n",myproc, (char*)((char*)key_size + 2*sizeof(uint64_t)), i, *(uint64_t*)((char*)key_size + 2*sizeof(uint64_t) + *key_size) );
		}

		//dprintf("i %llu count %llu\r\n",i, count);
        if(count <= datanum)
        {
			/*
            uint64_t len = std::min(chunk_size, count-start);
            int lgrp = loc_mem_to_lgrp (
                static_cast<Impl const*>(this)->locate(data+start, len));
			*/
			int lgrp = 0;
                task_queue::task_t task = 
                    // For debugging, last element is normally padding
                    {    i, data_index_e - data_index_s, (uint64_t)(data+data_index_s), lgrp };    
					//dprintf("before enqueue_seq len %llu data %llx\r\n", data_index_e - data_index_s, data+data_index_s);
					//dprintf("enqueue_seq debug_count %d\r\n", debug_count++);
            this->taskQueue->enqueue_seq (task, this->num_map_tasks, lgrp);
        }
    }

	//dprintf("num_map_tasks %llu num_threads %llu \r\n", num_map_tasks, num_threads);
    start_workers (&map_callback2, std::min(num_map_tasks, num_threads), "map"); 
}

/**
 * Dequeue the latest task and run it
 */
template<typename Impl, typename D, typename K, typename V, class Container>
void MapReduce<Impl, D, K, V, Container>::
map_worker(thread_loc const& loc, double& time, double& user_time, int& tasks)
{
    timespec begin = get_time();
    typename container_type::input_type t = container.get(loc.thread);    
    task_queue::task_t task;
    while (taskQueue->dequeue (task, loc)) {
        tasks++;
    	timespec user_begin = get_time();
	for (data_type* data = (data_type*)task.data; 
            data < (data_type*)task.data + task.len; ++data) {
            static_cast<Impl const*>(this)->map(*data, t);
        }
    	user_time += time_elapsed(user_begin);
    }

    container.add(loc.thread, t);
    time += time_elapsed(begin);
}

template<typename Impl, typename D, typename K, typename V, class Container>
void MapReduce<Impl, D, K, V, Container>::
map_worker2(thread_loc const& loc, double& time, double& user_time, int& tasks)
{
    timespec begin = get_time();
    typename container_type::input_type t = container.get(loc.thread);    
    task_queue::task_t task;

	int count = 0;

    while (taskQueue->dequeue (task, loc)) {
        tasks++;
    	timespec user_begin = get_time();

		//dprintf("In map_worker2 before map2.....\r\n");
		static_cast<Impl const*>(this)->map2(task.data, task.len, t);
		//dprintf("In map_worker2 after map2 count %d myproc %d.....\r\n", count++, myproc);
#if 0
	for (data_type* data = (data_type*)task.data; 
            data < (data_type*)task.data + task.len; ++data) {
            static_cast<Impl const*>(this)->map2(*data, t);
        }
#endif
		//break;
    	user_time += time_elapsed(user_begin);
    }

    container.add(loc.thread, t);
    time += time_elapsed(begin);
}

/**
 * Run reduce tasks and get final values. 
 */
template<typename Impl, typename D, typename K, typename V, class Container>
void MapReduce<Impl, D, K, V, Container>::run_reduce ()
{
    // Create tasks and enqueue...
    for (uint64_t i = 0; i < this->num_reduce_tasks; ++i) {
        task_queue::task_t task = {    i, 0, i, 0 };
        this->taskQueue->enqueue_seq(task, this->num_reduce_tasks);
    }

    start_workers (&reduce_callback, 
        std::min(this->num_reduce_tasks, num_threads), "reduce");
}

/**
 * Dequeue next reduce task and do it
 */
template<typename Impl, typename D, typename K, typename V, class Container>
void MapReduce<Impl, D, K, V, Container>::reduce_worker (
    thread_loc const& loc, double& time, double& user_time, int& tasks)
{
    timespec begin = get_time();

    task_queue::task_t task;
    while (taskQueue->dequeue (task, loc)) {
        tasks++;

        typename container_type::iterator i = container.begin(task.data);

        timespec user_begin = get_time();
        K key;
        reduce_iterator values;

        while(i.next(key, values))
        {
            if(values.size() > 0)
                static_cast<Impl const*>(this)->reduce(
                    key, values, this->final_vals[loc.thread]);
        }
        user_time += time_elapsed(user_begin);
    }

    time += time_elapsed(begin);
}

/**
 * Merge all reduced data 
 */
template<typename Impl, typename D, typename K, typename V, class Container>
void MapReduce<Impl, D, K, V, Container>::run_merge ()
{
    size_t total = 0;
    for(size_t i = 0; i < num_threads; i++) {
        total += this->final_vals[i].size();
    }

    std::vector<keyval>* final = new std::vector<keyval>[1];
    final[0].reserve(total);

    for(size_t i = 0; i < num_threads; i++) {
        final[0].insert(final[0].end(), this->final_vals[i].begin(), 
            this->final_vals[i].end());
    }

    delete [] this->final_vals;
    this->final_vals = final;
}

template<typename Impl, typename D, typename K, typename V, class Container>
void MapReduce<Impl, D, K, V, Container>::
merge_worker (thread_loc const& loc, double& time, double& user_time, int& tasks)
{
    // do nothing at all unless it turns out to be a bottleneck to merge in serial.
}


template<typename Impl, typename D, typename K, typename V, class Container>
int MapReduce<Impl, D, K, V, Container>::
run2 (char *data, uint64_t datanum, std::vector<keyval>& result)
{
    timespec begin;    
    timespec run_begin = get_time();
    // Initialize library
    get_time (begin);
	int showline = 10;

#if 0
    std::vector<D> data;
    uint64_t count;
    D chunk;

    // Run splitter to generate chunks
    get_time (begin);
    while (static_cast<Impl const*>(this)->split(chunk))
    {
        data.push_back(chunk);
    }
    count = data.size();
    print_time_elapsed("split phase", begin);
#endif

    // Compute task counts (should make this more adjustable) and then 
    // allocate storage
    this->num_map_tasks = std::min(datanum, this->num_threads) * 16;
    this->num_reduce_tasks = this->num_threads;
    dprintf ("num_map_tasks = %d\n", num_map_tasks);
    dprintf ("num_reduce_tasks = %d\n", num_reduce_tasks);

    container.init(this->num_threads, this->num_reduce_tasks);
    this->final_vals = new std::vector<keyval>[this->num_threads];
    for(uint64_t i = 0; i < this->num_threads; i++) {
        // Try to avoid a reallocation. Very costly on Solaris.
        this->final_vals[i].reserve(100);
    }
    print_time_elapsed("library init", begin);

    // Run map tasks and get intermediate values
    get_time (begin);
    run_map2(data, datanum);
    print_time_elapsed("map phase", begin);

    dprintf("In run2 proc %d, all map tasks are done, now scheduling reduce tasks\n", myproc);

    // Run reduce tasks and get final values
    get_time (begin);
    run_reduce();
    print_time_elapsed("reduce phase", begin);

    dprintf("In run2 proc %d, all reduce tasks are done, now scheduling merge tasks\n", myproc);

    get_time (begin);
    run_merge();
    print_time_elapsed("merge phase", begin);
    
    dprintf("In run2 proc %d, all merge tasks are done, now scheduling finish tasks\n", myproc);

    result.swap(*this->final_vals);
    
    // Delete structures
    delete [] this->final_vals;
    
    //print_time_elapsed("run time", run_begin);

	size_t keysizes = result.size();
	if(showline > keysizes)
	{
		showline = keysizes;
	}

	for(size_t i = 0; i < showline; i++)
	{
		dprintf("run2 proc %d %15s - %lu\n",myproc, result[result.size()-1-i].key.data, result[result.size()-1-i].val);
	}

	dprintf("In run2 proc %d, All work done..\r\n", myproc);

	//MPI_Barrier(MPI_COMM_WORLD);
	mapreduce_barrier();

    return 0;
}


template<typename Impl, typename D, typename K, typename V, class Container>
void MapReduce<Impl, D, K, V, Container>::run_dispatch (std::vector<keyval>& result)
{
    timespec begin;    

	unsigned int keysizes;
	std::vector<unsigned int> sendto;	 //each key send to which proc, this is the hash result, (0 ~ nprocs-1)

	int *sendbytes	= NULL;	 	//the bytes that current proc send to other procs.
	int *recvbytes	= NULL;	 	//the bytes that current proc recv from other procs.
	int *sendnum	= NULL;		//the num of keyvalue that current proc send to other procs.
	int *recvnum	= NULL;		//the num of keyvalue that current proc recv from other procs.
	int *sdispls 	= NULL;		//the send offset, see MPI_Alltoallv function get the detail.
	int *rdispls	= NULL;		//the recv offset, see MPI_Alltoallv function get the detail.

	char *snd_data_buffer 	= NULL;		//the current proc's keyvalues buffer, which will be send to the other procs.
	char *rcv_data_buffer 	= NULL;	 	//the recv buffer, which will be receive keyvalues from other procs.
	char **snd_data_index 	= NULL;		//used for indexing the snd_data_buffer.
	int snd_buffer_size 	= 0;		//the snd_data_buffer's size.	
	int rcv_buffer_size 	= 0;		//the rcv_data_buffer's size.
	int recv_total_datanum 	= 0;		//the number of keyvalue that current proc whill be receive.


    dprintf ("In run_dispatch, nprocs %d myproc %d\n", nprocs, myproc);
	dprintf ("result.size is %d\r\n", result.size());
	
	dprintf("proc %d wait dispatch\r\n", myproc);
	//MPI_Barrier(MPI_COMM_WORLD);
	mapreduce_barrier();

	dprintf("proc %d start dispatch\r\n", myproc);

    get_time (begin);

	sendbytes 	= new int[nprocs];
	recvbytes 	= new int[nprocs];
	sendnum 	= new int[nprocs];
	recvnum 	= new int[nprocs];
	sdispls 	= new int[nprocs];
	rdispls 	= new int[nprocs];
	snd_data_index = new char*[nprocs];

	assert(sendbytes != NULL);
	assert(recvbytes != NULL);
	assert(sendnum   != NULL);
	assert(recvnum   != NULL);
	assert(sdispls   != NULL);
	assert(rdispls   != NULL);
	assert(snd_data_index != NULL);

	memset(sendbytes, 0x00, nprocs * sizeof(int));
	memset(recvbytes, 0x00, nprocs * sizeof(int));
	memset(sendnum, 0x00, nprocs * sizeof(int));
	memset(recvnum, 0x00, nprocs * sizeof(int));
	memset(sdispls, 0x00, nprocs * sizeof(int));
	memset(rdispls, 0x00, nprocs * sizeof(int));
	memset(snd_data_index, 0x00, nprocs * sizeof(char*));

	keysizes = result.size();
	
	//hash each key to (0 ~ nprocs-2), why not (1~nproc-1), 
	//because MPI_Alltoall or MPI_Alltoallv communicator is newcomm, 
	//not MPI_COMM_WORLD communicator, so must be 0~nprocs-2, the 0 indicate proc1 ... and so on.
	for(size_t i = 0; i < keysizes; i++)
	{
		int h;
		h = container.custom_hash(result[i].key) % (nprocs-1); 
		sendto.push_back(h);
	}

	//call user-defined getkey() and getvalue() function, get the length of the key and value.
	//the aim is to build the snd_data_buffer.
	for(size_t i = 0; i < keysizes; i++)
	{
		char *key;
		char *value;
		int key_len;
		int value_len;
		int kv_size;

		key = static_cast<Impl const*>(this)->getkey(result[i].key, key_len);
		value = static_cast<Impl const*>(this)->getvalue(result[i].val, value_len);

		kv_size = 2*sizeof(int) + key_len + value_len;
		sendbytes[sendto[i]] += kv_size;
		sendnum[sendto[i]]++;
	}
	

	//calculate the send offset.
	sdispls[0] = 0;
	snd_buffer_size = sendbytes[0];
	for (int i = 1; i < nprocs-1; i++) {
		sdispls[i] = sdispls[i-1] + sendbytes[i-1];
		snd_buffer_size += sendbytes[i];
	}

	for (int i = 0; i < nprocs; i++) {
		//printf("proc%d sendbytes[%d] %d\r\n",myproc,i, sendbytes[i]);
	}
	//printf("\r\n\r\n");


	//MPI_Alltoall(sendbytes, 1, MPI_INT, recvbytes, 1, MPI_INT, comm);
	//MPI_Alltoall(sendnum, 1, MPI_INT, recvnum, 1, MPI_INT, comm);
	MPI_Alltoall(sendbytes, 1, MPI_INT, recvbytes, 1, MPI_INT, static_cast<Impl const*>(this)->newcomm);
	MPI_Alltoall(sendnum, 1, MPI_INT, recvnum, 1, MPI_INT, static_cast<Impl const*>(this)->newcomm);

	for (int i = 0; i < nprocs; i++) {
		//printf("proc%d recvbytes[%d] %d\r\n",myproc,i, recvbytes[i]);
	}

	//calculate the recv offset and recv_total_datanum.
	rdispls[0] = 0;
	rcv_buffer_size = recvbytes[0];
	recv_total_datanum = recvnum[0];
	for (int i = 1; i < nprocs-1; i++) {
		rdispls[i] = rdispls[i-1] + recvbytes[i-1];
		rcv_buffer_size += recvbytes[i];
		recv_total_datanum += recvnum[i];
	}
	
	snd_data_buffer = new char[snd_buffer_size];
	rcv_data_buffer = new char[rcv_buffer_size];

	assert(snd_data_buffer != NULL);
	assert(rcv_data_buffer != NULL);
	
	printf("snd_buffer_size %d\r\n", snd_buffer_size);
	printf("rcv_buffer_size %d\r\n", rcv_buffer_size);
	dprintf ("result print over.\r\n");

	//snd_data_index used for build snd_data_buffer.
	for (int i = 0; i < nprocs-1; i++)
	{
		snd_data_index[i] = snd_data_buffer + sdispls[i];
	}

	//copy the keyvalues content to the snd_data_buffer.
	for(int i = 0; i < keysizes; i++)
	{
		char *key;
		char *value;
		int key_len;
		int value_len;
		int kv_size;

		int send;

		key = static_cast<Impl const*>(this)->getkey(result[i].key, key_len);
		value = static_cast<Impl const*>(this)->getvalue(result[i].val, value_len);

		send = sendto[i];

		memcpy(snd_data_index[send], &key_len, sizeof(int));
		snd_data_index[send] += sizeof(int);

		memcpy(snd_data_index[send], &value_len, sizeof(int));
		snd_data_index[send] += sizeof(int);

		memcpy(snd_data_index[send], key, key_len);
		snd_data_index[send] += key_len;

		memcpy(snd_data_index[send], value, value_len);
		snd_data_index[send] += value_len;
	}

	
	//exchange the keyvalues between each proc.
	//MPI_Alltoallv(snd_data_buffer, sendbytes, sdispls, MPI_BYTE, rcv_data_buffer, 
	// 	recvbytes, rdispls, MPI_BYTE, comm);

	MPI_Alltoallv(snd_data_buffer, sendbytes, sdispls, MPI_BYTE, rcv_data_buffer, 
	 	recvbytes, rdispls, MPI_BYTE, static_cast<Impl const*>(this)->newcomm);

	//for debug, check the keyvalue that received.
#if 0
	char *index = rcv_data_buffer;
	for(int i = 0; i < 10; i++)
	{
		uint64_t *key_index = (uint64_t*)index;
		uint64_t *value_index = (uint64_t*)(index + sizeof(uint64_t));
		char *key, *value;

		printf("keysize %lu\r\n", *key_index);
		printf("valuesize %lu\r\n", *value_index);

		key = index + 2*sizeof(uint64_t);
		value = key + *key_index;

		key[strlen(key)] = '\0';
		printf("key %s\r\n", key);
		printf("value %lu\r\n", *(int*)value);
	}
#endif

	//result.clear();
	//MPI_Barrier(MPI_COMM_WORLD);
	mapreduce_barrier();
	
	

	dprintf("proc %d recv_total_datanum is %d\r\n",myproc, recv_total_datanum);
	run2 (rcv_data_buffer, recv_total_datanum, result);

	//MPI_Barrier(MPI_COMM_WORLD);
	mapreduce_barrier();
	

    print_time_elapsed_proc(myproc, "run_dispatch phase",begin);

free_memory:
	delete []sendbytes;
	delete []recvbytes;
	delete []sendnum;
	delete []recvnum;
	delete []sdispls;
	delete []rdispls;
	delete []snd_data_index;

	delete []snd_data_buffer;
	delete []rcv_data_buffer;
}

template<typename Impl, typename D, typename K, typename V, class Container>
void MapReduce<Impl, D, K, V, Container>::start_workers (void (*func)(void*, thread_loc const&), int num_threads, char const* stage)
{
    thread_arg_t* th_arg_array = new thread_arg_t[num_threads];
    thread_arg_t** th_arg_ptrarray = new thread_arg_t*[num_threads];
    
    thread_arg_t args = { this, 0, 0, 0 };
    for (int thread = 0; thread < num_threads; ++thread) 
    {
        th_arg_array[thread] = args;
        th_arg_ptrarray[thread] = &(th_arg_array[thread]);        
    }
    
    CHECK_ERROR (threadPool->set(func, (void **)th_arg_ptrarray, num_threads));
    // Start worker threads
    CHECK_ERROR (threadPool->begin());                
    //dprintf("Status: All %d threads have been created\n", num_threads);    
    // Barrier, wait for all threads to finish.
    CHECK_ERROR (threadPool->wait());            

#ifdef TIMING
    double user_time = 0, work_time = 0, max_user_time = 0, 
        min_user_time=std::numeric_limits<double>::max(), max_work_time=0, 
        min_work_time=std::numeric_limits<double>::max();
    for (int thread = 0; thread < num_threads; ++thread)
    {
        //dprintf("Thread %d: ran %d in %.3f\n", thread, 
         //   th_arg_array[thread].tasks, th_arg_array[thread].time);
        user_time += th_arg_array[thread].user_time;
        min_user_time = std::min(min_user_time, th_arg_array[thread].user_time);
        max_user_time = std::max(max_user_time, th_arg_array[thread].user_time);
        work_time += th_arg_array[thread].time;
        min_work_time = std::min(min_work_time, th_arg_array[thread].time);
        max_work_time = std::max(max_work_time, th_arg_array[thread].time);
    }
#if 0
    if(max_user_time > 0)
        fprintf (stderr, "%s avg user time: %.3f    (%.3f, %.3f)\n", 
            stage, user_time / num_threads, min_user_time, max_user_time);
    if(max_work_time > 0)
        fprintf (stderr, "%s avg thread time: %.3f    (%.3f, %.3f)\n", 
            stage, work_time / num_threads, min_work_time, max_work_time);
#endif
#endif

    delete [] th_arg_ptrarray;
    delete [] th_arg_array;
    
    //dprintf("Status: All tasks have completed\n"); 
}

template<typename Impl, typename D, typename K, typename V, 
    class Container = hash_container<K, V, buffer_combiner> >
class MapReduceSort : public MapReduce<Impl, D, K, V, Container>
{
public:
    typedef typename MapReduce<Impl, D, K, V, Container>::keyval keyval;

protected:
    
    // default sorting order is by key. User can override.
    bool sort(keyval const& a, keyval const& b) const { return a.key < b.key; }

    struct sort_functor {
        MapReduceSort* mrs;
        sort_functor(MapReduceSort* mrs) : mrs(mrs) {}
        bool operator()(keyval const& a, keyval const& b) const { 
            return static_cast<Impl const*>(mrs)->sort(a, b); 
        }
    };

    virtual void run_merge ()
    {
        // how many lists to merge in a single task.
        static const int merge_factor = 2;    
        int merge_queues = this->num_threads;
    
        // First sort each queue in place
        for(int i = 0; i < merge_queues; i++)
        {
            task_queue::task_t task = 
                { i, 0, (uint64_t)&this->final_vals[i], 0 };
            this->taskQueue->enqueue_seq(task, merge_queues);
        }
        start_workers(&this->merge_callback, this->num_threads, "merge");

        // Then merge
        std::vector<keyval>* merge_vals;
        while (merge_queues > 1) {
            uint64_t resulting_queues = 
                (uint64_t)std::ceil(merge_queues / (double)merge_factor);
        
            // swap queues
            merge_vals = this->final_vals;
            this->final_vals = new std::vector<keyval>[resulting_queues];
        
            // distribute tasks into task queues using locality information 
            // if provided.
            int queue_index = 0;
            for(uint64_t i = 0; i < resulting_queues; i++)
            {
                int actual = std::min(merge_factor, merge_queues-queue_index);
                task_queue::task_t task 
                    = { i,
                        (uint64_t)actual,
                        (uint64_t)&merge_vals[queue_index],
                        0 };
                int lgrp = loc_mem_to_lgrp (&merge_vals[queue_index][0]);        
                // For debugging, normally this is padding.
                task.pad = lgrp;                    
                this->taskQueue->enqueue_seq (task, resulting_queues, lgrp);
                queue_index += actual;
            }

            // Run merge tasks and get merge values.
            start_workers (&this->merge_callback, 
                std::min(resulting_queues, this->num_threads), "merge");

            delete [] merge_vals;
            merge_queues = resulting_queues;
        }

        assert(merge_queues == 1);
    }

    virtual void merge_worker (thread_loc const& loc, double& time, 
        double& user_time, int& tasks)
    {
        timespec begin = get_time();
        task_queue::task_t task;
        while (this->taskQueue->dequeue (task, loc)) {
            tasks++;
            std::vector<keyval>* vals = (std::vector<keyval>*)task.data;
            uint64_t length = task.len;
            uint64_t out_index = task.id;

            if(length == 0)
            {
                // this case really just means sort my list in place. 
                // stable_sort ensures that the order of same keyvals with 
                // the same key emitted in reduce remains the same in sort
                std::stable_sort(vals->begin(), vals->end(), sort_functor(this));
            }
            else if(length == 1)
            {
                // if only one list, don't merge, just move over.
                (*vals).swap(this->final_vals[out_index]);
            }
            else if(length == 2)
            {
                // stl merge is nice and fast for 2.
                this->final_vals[out_index].resize(vals[0].size()+vals[1].size());
                std::merge(vals[0].begin(), vals[0].end(), 
                    vals[1].begin(), vals[1].end(), 
                        this->final_vals[out_index].begin(), sort_functor(this));
            }
            else
            {
                // for more, do a multiway merge.
                assert(0);
            }
        }
        time += time_elapsed(begin);
    }
};

#endif // MAP_REDUCE_H_

// vim: ts=8 sw=4 sts=4 smarttab smartindent
