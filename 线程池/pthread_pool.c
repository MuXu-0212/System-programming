#include "pthread_pool.h"

//功能:初始化线程池
//参数 ：thread_pool *pool 你要初始化哪一个线程池
//参数 ：unsigned int thread_number 线程池一初始化的时候有几条线程
bool Pthread_Pool_Init(thread_pool *pool, unsigned int thread_number)
{
	pthread_mutex_init(&pool->lock, NULL);//初始化互斥锁
	pthread_cond_init(&pool->cond, NULL); //初始化条件变量
	pool->shutdown = false;   //初始标志位 为 假 代表不销毁 true代表销毁

	//初始化任务链表的头节点,方便后面添加任务进行使用
	pool->task_list = (struct task*)malloc(sizeof(struct task));
	if (pool->task_list == NULL)
	{
		printf("申请任务链表头节点失败\n");
		return false;
	}

	//初始化用于保存线程池的所有的线程号
	pool->tids = (pthread_t *)malloc(sizeof(pthread_t)*MAX_ACTIVE_THREADS);
	if (pool->tids == NULL)
	{
		printf("申请储存线程号堆空间失败\n");
		return false;
	}

	//对任务链表的头节点进行初始化
	pool->task_list->next = NULL;

	//对等待的任务数量进行初始化
	pool->max_waiting_tasks = MAX_WAITING_TASKS;
	//需要处理的任务数量进行初始化
	pool->waiting_tasks = 0;

	//对初始化的线程数量进行初始化
	pool->active_threads = thread_number;
	
	//循环创建线程
	int ret;
	for (size_t i = 0; i < thread_number; i++)
	{	
		ret = pthread_create(&((pool->tids)[i]), NULL, routine, (void *)pool);
		if(ret != 0)
		{
			printf("%s\n",strerror(ret));
			return false;
		}
	}
	
	return true;
}

//函数功能：添加任务
//参数: thread_pool *pool 你需要把任务添加到哪个线程池当中
//参数: void *(*do_task)(void *arg) 你需要添加的任务
//参数: void *arg 传递给该任务的参数
bool Add_Task(thread_pool *pool, void *(*do_task)(void *arg), void *arg)
{
	//初始化一个新节点出来
	struct task *new_task = (struct task*)malloc(sizeof(struct task));
	if (new_task == NULL)
	{
		printf("申请任务节点失败\n");
		return false;
	}
	
	//对新节点的数据域与指针域进行初始化
	new_task->do_task = do_task; //把一个任务添加到链表的节点当中
	new_task->arg     = arg;     //把传递进来的参数写到链表当中去
	new_task->next    = NULL;

	//在添加任务之前需要进行上锁操作
	pthread_mutex_lock(&(pool->lock));

	//判断任务链表当中是否有1000个任务,如果有则释放刚刚申请出来的任务，
	if(pool->waiting_tasks >= MAX_WAITING_TASKS)
	{
		//提前让出CPU资源
		pthread_mutex_unlock(&(pool->lock));
		fprintf(stderr,"任务太多无法添加\n");
		free(new_task);
		return false;
	}
	//如果该等待任务不足1000个,则尾插
	struct task *tmp = pool->task_list;
	//寻找到最后一个节点
	while(tmp->next != NULL)
	{
		tmp = tmp->next;
	}
	//让最后一个节点指向新的节点
	tmp->next = new_task;
	//当前待处理的任务进行 +1
	pool->waiting_tasks++;
	//处理完该节点之后解锁
	pthread_mutex_unlock(&(pool->lock));
	//唤醒一条线程开始工作
	pthread_cond_signal(&(pool->cond));

	//添加任务成功
	return true;
}

void headler(void *arg)
{
	printf("%ld 进入到解锁的函数\n",pthread_self());
	pthread_mutex_unlock((pthread_mutex_t *)arg);
}

//线程处理任务函数 需要传递的参数最好是线程池首地址
void *routine(void *arg)
{
	//1.先把数据类型强转
	thread_pool *pool = (thread_pool *)arg;
	struct task *p = NULL;
	while(1)
	{
		//防止线程被意外取消,会出现死锁的情况
		//headler: 处理死锁的函数
		pthread_cleanup_push(headler, (void *)&pool->lock);

		//上锁,因为任务链表是一个共享资源
		pthread_mutex_lock(&(pool->lock));
		
		//当线程池当中没有任务并且线程池不被销毁的情况下
		if (pool->waiting_tasks == 0 && pool->shutdown == false)
		{
			//等待条件变量(任务)
			pthread_cond_wait(&(pool->cond),&(pool->lock));
		}
		//如果线程池当中没有任务 并且 线程池将要被销毁的情况下
		if(pool->waiting_tasks == 0 && pool->shutdown == true)
		{
			//先解锁
			pthread_mutex_unlock(&(pool->lock));
			//退出线程
			pthread_exit(NULL);
		}
		//如果线程执行到该位置,要运行任务
		//从链表头的位置取下第一个节点然后进行运行.
		p = pool->task_list->next;
		pool->task_list->next = p->next; //让取下P节点
		p->next = NULL;

		//待执行的任务个数 -1
		pool->waiting_tasks--;
		
		//解锁
		pthread_mutex_unlock(&(pool->lock));
		pthread_cleanup_pop(0);
		//防止执行任务的过程被意外取消,则需要设置取消的状态,设置为不可取消的状态
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

		//执行任务,(函数调用)
		(p->do_task)(p->arg);

		//当任务执行完毕之后,需要设置为可取消.
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
		//该任务已经被消费完毕,释放该节点
		free(p);
	}
}

//往线程池当中添加线程
//thread_pool *pool 线程池
//unsigned int number 要往该线程池当中添加多少条线程
int Add_Pthread(thread_pool *pool, unsigned int number)
{
	//判断传递的参数是否有意义,代表查看当前有多少条活跃线程
	if (number == 0)
	{
		return pool->active_threads;
	}

	//把原有的线程的数量 加上新添加的数量
	unsigned int total_threads = pool->active_threads + number;
	unsigned int value = 0;  //添加成功的线程数目
	int ret;
	//最多只能够创建 19
	for (size_t i = pool->active_threads; i < total_threads && i < MAX_ACTIVE_THREADS ; i++)
	{
		ret = pthread_create(&(pool->tids)[i], NULL, routine, (void*)pool);
		if (ret != 0)
		{
			printf("%s\n",strerror(ret));
			break;
		}
		value++;
	}
	//真正的添加后的数量
	pool->active_threads += value;
	
	return pool->active_threads;
}

//把线程池当中的线程删除
int Delete_Pthread(thread_pool *pool, unsigned int delnumber)
{
	if (delnumber == 0)
	{
		return pool->active_threads;
	}

	//用线程池当中原有的线程数量减去 要删除的数量
	int temp = pool->active_threads - delnumber;
	//如果该线程池的线程数量要小于0 至少保留一条线程
	temp = temp > 0 ? temp : 1;
	int i;
	for (i = pool->active_threads-1 ; i > temp - 1; i--)
	{
		//从最后一条线程开始取消
		errno = pthread_cancel(pool->tids[i]);
		if (errno != 0)
		{
			break;
		}
	}

	//如果说以上代码都没有出错的话,i 就应该等于 tmp -1;
	if (i == pool->active_threads-1) //删除失败
	{
		return -1;
	}
	else
	{ 
		//删除一部分或全部删除完成
		pool->active_threads = i + 1;
		return pool->active_threads;
	}
	
}

//销毁整个线程池
bool Destroy_Pool(thread_pool *pool)
{
	//1.把线程池销毁的标志位置成true
	pool->shutdown = true;

	//2.唤醒所有在沉睡的线程
	pthread_cond_broadcast(&pool->cond);

	for (size_t i = 0; i < pool->active_threads; i++)
	{
		errno = pthread_join(pool->tids[i], NULL);
		if (errno != 0)
		{
			printf("join [%ld] error %s\n", pool->tids[i], strerror(errno));
		}
		else
		{
			printf("[%ld] exit\n", pool->tids[i]);
		}
	}

	//如果执行到该位置代表所有的线程已退出
	free(pool->task_list);
	free(pool->tids);
}




//用户自定义任务
//void *arg 秒数
void *mytask(void *arg)
{
	int n = *(int *)arg;

	printf("[%ld][%s] ======>正在执行任务 %d sec ..\n",\
	pthread_self(), __FUNCTION__ , n);

	sleep(n);

	printf("[%ld][%s] ======>任务执行完毕\n",\
	pthread_self(), __FUNCTION__);
	
	return NULL;
}

int main(int argc, char const *argv[])
{
	srand(time(NULL));
	thread_pool tmp;
	thread_pool *pool = &tmp;
	Pthread_Pool_Init(pool, 2);

	//添加任务到任务链表当中
	int a = rand()%20+5;
	int b = rand()%20+5;
	int c = rand()%20+5;

	Add_Task(pool, mytask, (void *)&a);
	Add_Task(pool, mytask, (void *)&b);
	Add_Task(pool, mytask, (void *)&c);

	sleep(5);

	Add_Task(pool, mytask, (void *)&a);
	Add_Task(pool, mytask, (void *)&b);
	Add_Task(pool, mytask, (void *)&c);

	Add_Pthread(pool, 3);

	printf("添加后的活跃线程数: %d \n",pool->active_threads);

	sleep(20);

	Delete_Pthread(pool, 3);

	printf("删除后的线程数： %d \n",pool->active_threads);

	Destroy_Pool(pool);

	return 0;
}
