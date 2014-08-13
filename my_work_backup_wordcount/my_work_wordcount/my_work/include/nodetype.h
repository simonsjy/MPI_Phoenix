/*========================================================================
#   FileName: nodetype.h
#     Author: simon
#      Email: 297873898@qq.com
#    Version: 1.0.0
#   HomePage: http://www.xxxxxxxxx.cn
# LastChange: 2014-06-05 19:47:43
========================================================================*/
#ifndef __NODETYPE_H__
#define __NODETYPE_H__


#include <sys/types.h>

#define MAX_NAME_LEN 64

typedef void (*map_t)(map_args_t);

class NodeType_Args {
	public:
		NodeType_Args();
		~NodeType_Args();

		char nodetype[MAX_NAME_LEN];

	private:
		
};











#endif


