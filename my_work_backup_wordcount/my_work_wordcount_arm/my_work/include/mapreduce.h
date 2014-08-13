/*========================================================================
#   FileName: mapreduce.h
#     Author: wangjian
#      Email: kuangrenyx@qq.com
#    Version: 1.0.0
#   HomePage: http://www.xxxxxxxxx.cn
# LastChange: 2013-06-06 10:57:26
========================================================================*/
#ifndef __MAPREDUCE_H__
#define __MAPREDUCE_H__

#include <mpi.h>
#include "work_mapreduce.h"
#include "iterator.h"

#define FILECHUNK		20	
#define MAXLINE			1024
#define MAX_NAME_LEN	128


/*
 *This function is used to split the files by master node
 */
class MasterControl;
typedef void (*split_file)(char *filename);


/*
 *nfile,files:Program input file or directory
 *recurse,readflag:The method of processing a file or directory
 *recurse:when dealing with directory,whether recursive processing subdirectories or files,0:not process,1:process
 *readflag:when dealing with trivial file,whether to read each line of the file as a file
 */
typedef struct inputfile_t {
	int 	nfile;
	char 	**files;
	int 	recurse;
	int 	readflag;
}inputfile_arg;

/*
 *nfile:the number of files to be processed
 *files:array of file names
 *split_file_callback:User-defined split function, 
 *This callback function is called internally by the framework
 */
typedef struct master_control_arg_t {
	int 	nfile;
	char 	**files;

	split_file split_file_callback;
}master_control_arg;



typedef struct MapReduce_ARG_t {

	inputfile_arg			inputfile;
	master_control_arg 		mca;
	worknode_control_arg 	work_ctlarg;

}MapReduce_ARG;

class MapReduce {
	public:
		MapReduce();
		~MapReduce();

		static int getProcId(){ return myproc;}

		void schedule(MapReduce_ARG *mr_arg);

	private:
		static int myproc;	//myproc id
		int nprocs;			//current proc num

		int nfile;
		char **files;

		int ranks[128];

		MPI_Comm 	comm;
		MPI_Group	group;
		MPI_Group	newgroup;
		MPI_Comm 	newcomm;
		

		int scanfiles(char *str, int &nfile, int &maxfile, char **&files);

		void findfiles(char *str, int recurse, int readflag,
				int &nfile, int &maxfile, char **&files);

		void addfiles(char *str, int readflag, 
				int &nfile, int &maxfile, char **&files);

		void bcastfiles(int &nfile, char **&files);
};


#endif
