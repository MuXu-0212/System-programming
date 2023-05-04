#include "thread_pool.h"

void handler(void *arg)
{
	printf("[%u] is ended.\n",(unsigned)pthread_self());
	pthread_mutex_unlock((pthread_mutex_t *)arg);//解锁
}

void *routine(void *arg)  //子线程的实现载体（函数）
{
	#ifdef DEBUG
	printf("[%u] is started.\n",
		(unsigned)pthread_self());
	#endif

	thread_pool *pool = (thread_pool *)arg;  //接收主线程传递过来的参数（线程池指针）
	struct task *p;

	while(1)
	{
		/*
		** push a cleanup functon handler(), make sure that
		** the calling thread will release the mutex properly
		** even if it is cancelled during holding the mutex.
		**
		** NOTE:
		** pthread_cleanup_push() is a macro which includes a
		** loop in it, so if the specified field of codes that 
		** paired within pthread_cleanup_push() and pthread_
		** cleanup_pop() use 'break' may NOT break out of the
		** truely loop but break out of these two macros.
		** see line 56 below.
		*/
		//================================================//
		pthread_cleanup_push(handler, (void *)&pool->lock);   //将注册函数handler压栈，如果出现异常退出，在退出前就会出栈执行handler函数
																	//防止加锁后没来的及解锁就意外结束
		pthread_mutex_lock(&pool->lock);//加锁
		//================================================//

		// 1, no task, and is NOT shutting down, then wait
		while(pool->waiting_tasks == 0 && !pool->shutdown)//当前线程池任务数为0切标记销毁为假
		{
			pthread_cond_wait(&pool->cond, &pool->lock);//进入条件变量等待
		}

		// 2, no task, and is shutting down, then exit
		if(pool->waiting_tasks == 0 && pool->shutdown == true)//当前线程池任务数为0切标记销毁为真
		{
			pthread_mutex_unlock(&pool->lock); //解锁
			pthread_exit(NULL); // CANNOT use 'break';  //退出子线程
		}

		// 3, have some task, then consume it
		p = pool->task_list->next; //找到头节点的下一个节点，待执行任务的第一个任务 ,用p来记录
		pool->task_list->next = p->next;   //删除任务列表中的马上要执行的节点
		pool->waiting_tasks--;   //当前待执行任务-1

		//================================================//
		pthread_mutex_unlock(&pool->lock);
		pthread_cleanup_pop(0);   //正常执行到这里，说明没有出现意外情况，不执行
		//================================================//

		//任务执行
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);   //防止主线程取消（屏蔽取消命令）
		(p->do_task)(p->dest,p->src); //执行任务
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);//可以主线程取消

		free(p);//释放刚刚执行的任务节点
	}

	pthread_exit(NULL);
}


//初始化线程池
bool init_pool(thread_pool *pool, unsigned int threads_number)
{
	pthread_mutex_init(&pool->lock, NULL); //互斥锁的初始化 
	pthread_cond_init(&pool->cond, NULL);//条件变量初始化

	pool->shutdown = false;  ////线程池不标记销毁，要使用
	pool->task_list = malloc(sizeof(struct task));    //堆空间申请一个任务链表的头节点
	pool->tids = malloc(sizeof(pthread_t) * MAX_ACTIVE_THREADS);//堆空间申请能够放置最多线程的线程id的空间

	if(pool->task_list == NULL || pool->tids == NULL) //堆空间申请失败，线程池初始化失败，返回false
	{
		perror("allocate memory error");
		return false;
	}

	pool->task_list->next = NULL;  //初始化任务头节点

	pool->max_waiting_tasks = MAX_WAITING_TASKS;   //设置最大等待任务数1000 
	pool->waiting_tasks = 0; //正在等待任务数为0
	pool->active_threads = threads_number;  //通过传参设置初始化的活跃线程数

	int i;
	for(i=0; i<pool->active_threads; i++)//创建线程
	{
		if(pthread_create(&((pool->tids)[i]), NULL,
					routine, (void *)pool) != 0)  //把线程池的指针传递给子线程，routine子线程的实现函数
		{
			perror("设置线程失败\n");
			return false;
		}

		#ifdef DEBUG     //调试作用
		printf("[%u]:[%s] ==> tids[%d]: [%u] 被创建.\n",
			(unsigned)pthread_self(), __FUNCTION__,
			i, (unsigned)pool->tids[i]);
		#endif
	}

	return true;
}

//投放任务
bool add_task(thread_pool *pool,  //线程池
	      	  void (*do_task)(char *dest_file,char *src_file),//要投放的新任务
	      	  char *dest,char *src)  //往投放的任务中传递参数
{
	struct task *new_task = malloc(sizeof(struct task));  //新建任务节点（给新的任务申请堆空间）
	if(new_task == NULL)
	{
		perror("分配内存失败\n");
		return false;
	}

	//对新建任务节点初始化
	new_task->do_task = do_task;   
	strcpy(new_task->dest,dest);
	strcpy(new_task->src,src);
	new_task->next = NULL;

	//============ LOCK =============//
	pthread_mutex_lock(&pool->lock);  //加互斥锁（防止多个任务同时进行投放，导致混乱）
	//===============================//

	if(pool->waiting_tasks >= MAX_WAITING_TASKS)  //判断当前的等待任务数如果大于最大等待数，解锁走人
	{
		pthread_mutex_unlock(&pool->lock);

		fprintf(stderr, "任务过多\n");
		free(new_task);

		return false;
	}
	
	struct task *tmp = pool->task_list;   //尾部插入
	while(tmp->next != NULL)
		tmp = tmp->next;

	tmp->next = new_task;
	pool->waiting_tasks++;    //等待数加1

	//=========== UNLOCK ============//
	pthread_mutex_unlock(&pool->lock);  //任务添加成功，解锁
	//===============================//

	#ifdef DEBUG     //调试
	printf("[%u][%s] ==> 添加了一个新任务\n",
		(unsigned)pthread_self(), __FUNCTION__);
	#endif

	pthread_cond_signal(&pool->cond);   //唤醒单条线程来执行投放的任务
	return true;
}

//增加线程池的活跃线程
int add_thread(thread_pool *pool, unsigned additional_threads)
{
	if(additional_threads == 0)
		return 0;

	unsigned total_threads =
			pool->active_threads + additional_threads;   //当前的活跃线程数加上新建入的线程数

	int i, actual_increment = 0;
	for(i = pool->active_threads; 
	    i < total_threads && i < MAX_ACTIVE_THREADS;   //两者取其小
	    i++)
	{
		if(pthread_create(&((pool->tids)[i]),
					NULL, routine, (void *)pool) != 0)      //创建新的线程
		{
			perror("添加线程失败\n");

			// no threads has been created, return fail
			if(actual_increment == 0)
				return -1;

			break;
		}
		actual_increment++; 

		#ifdef DEBUG
		printf("[%u]:[%s] ==> tids[%d]: [%u] 被创建.\n",
			(unsigned)pthread_self(), __FUNCTION__,
			i, (unsigned)pool->tids[i]);
		#endif
	}

	pool->active_threads += actual_increment;   //重新记录线程池的当前活跃线程数
	return actual_increment;   //返回实际新增的线程数
}

//删除线程池活跃线程
int remove_thread(thread_pool *pool, unsigned int removing_threads)
{
	if(removing_threads == 0)
		return pool->active_threads;

	int remaining_threads = pool->active_threads - removing_threads;
	remaining_threads = remaining_threads > 0 ? remaining_threads : 1;  //保证删除后线程池中最少有一条线程

	int i;
	for(i=pool->active_threads-1; i>remaining_threads-1; i--)
	{
		errno = pthread_cancel(pool->tids[i]); //取消线程

		if(errno != 0)//如果取消线程失败
			break;

		#ifdef DEBUG     //调试
		printf("[%u]:[%s] ==> cancelling tids[%d]: [%u]...\n",
			(unsigned)pthread_self(), __FUNCTION__,
			i, (unsigned)pool->tids[i]);
		#endif
	}

	if(i == pool->active_threads-1)
		return -1;
	else
	{
		pool->active_threads = i+1;   //下标与实际个数差1  ，重新记录线程池的当前活跃线程数
		return i+1;  //返回当前活跃线程数
	}
}

bool destroy_pool(thread_pool *pool)
{
	// 1, activate all threads
	pool->shutdown = true;  //线程池销毁标记为真，就要真的销毁线程池
	pthread_cond_broadcast(&pool->cond); //唤醒所有的等待条件变量

	// 2, wait for their exiting
	int i;
	for(i=0; i<pool->active_threads; i++)
	{
		errno = pthread_join(pool->tids[i], NULL);    //回收所有的活跃子线程
		if(errno != 0)
		{
			printf("join tids[%d] error: %s\n",
					i, strerror(errno));
		}
		else
			printf("[%u] 已加入\n", (unsigned)pool->tids[i]);
	}

	// 3, free memories
	free(pool->task_list);  //释放任务节点堆空间
	free(pool->tids);     //释放存放tid的堆空间
	free(pool);          //释放申请线程池的堆空间

	return true;
}
