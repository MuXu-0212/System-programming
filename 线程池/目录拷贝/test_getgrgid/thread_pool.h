#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include<fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <errno.h>
#include <pthread.h>

#include <sys/stat.h>
#include <sys/mman.h>
#include <stdbool.h>

#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <dirent.h>
#include "thread_pool.h"

#define MAX_WAITING_TASKS	1000    //最大任务量
#define MAX_ACTIVE_THREADS	20      //最大线程数

struct task  //任务的结构体
{
	void (*do_task)(char *dest_file,char *src_file);     //任务的实现
	char dest[128];        //要传递给任务实现函数的参数
	char src[128];
	struct task *next;   
};


//线程池的结构体
typedef struct thread_pool
{
	pthread_mutex_t lock;    //锁
	pthread_cond_t  cond;    //条件变量

	bool shutdown;      //线程池销毁标记

	struct task *task_list;   //任务链表的头节点

	pthread_t *tids;    //指向存放线程id的数组
 
	unsigned max_waiting_tasks;   //线程池最大等待任务的个数 
	unsigned waiting_tasks;     //线程池等待任务的个数 
	unsigned active_threads;    // 当前活跃线程个数
}thread_pool;


bool init_pool(thread_pool *pool, unsigned int threads_number);   //初始化线程池
				//pool线程池变量        初始化线程池时添加的线程数量
				
bool add_task(thread_pool *pool, void (*do_task)(char *dest_file,char *src_file), char *dest,char *src);//添加任务
				//给pool线程池添加任务   添加任务的实现函数      传递给任务的参数
		
int  add_thread(thread_pool *pool, unsigned int additional_threads_number);//给线程池pool添加additional_threads_number条线程
					
int  remove_thread(thread_pool *pool, unsigned int removing_threads_number);//删除线程池pool中的removing_threads_number条线程

bool destroy_pool(thread_pool *pool);//销毁线程池pool

void *routine(void *arg);//线程的实现函数


#endif
