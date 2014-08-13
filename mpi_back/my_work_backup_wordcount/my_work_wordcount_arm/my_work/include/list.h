/*========================================================================
#   FileName: list.h
#     Author: wangjian
#      Email: kuangrenyx@qq.com
#    Version: 1.0.0
#   HomePage: http://www.xxxxxxxxx.cn
# LastChange: 2013-10-23 21:27:36
========================================================================*/
/**
 *The list.h define "Two-way non-circular list"   
 *The user maintain the head and tail pointers
 */
#ifndef __LIST_H__
#define __LIST_H__

#define list_entry(ptr, type, member) \
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

typedef struct list_head {
	struct list_head *next;
	struct list_head *prev;
}list_head;


static inline void init_list_head(list_head *list)
{
	list->next = NULL;
	list->prev = NULL;
}

static inline void list_add_head(list_head *add, list_head *head)
{
	if(head != NULL)
	{
		add->prev 	= NULL;
		add->next	= head;
		head->prev 	= add;
	}
	init_list_head(add);
}

static inline void list_add_tail(list_head *add, list_head *tail)
{
	if(tail != NULL)
	{
		add->prev 	= tail;
		add->next	= NULL;
		tail->next 	= add;
	}
	init_list_head(add);
}

static inline list_head *list_remove_head(list_head *head)
{
	list_head *list = NULL;

	if(head != NULL && head->next != NULL)
	{
		list = head->next;
		head->next->prev = NULL;
	}

	head->next = NULL;
	return list;
}

static inline list_head *list_remove_tail(list_head *tail)
{
	list_head *list;

	if(tail != NULL && tail->prev != NULL)
	{
		list = tail->prev;
		list->next = NULL;
	}

	tail->prev = NULL;
	return list;
}

#endif

