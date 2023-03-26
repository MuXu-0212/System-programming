#include "head.h"

void *Fun(void *arg)
{
	int *p = (int*)arg;
	*(p+2) = *(p+1)+*p;

	//void *retval : 功能与 return 一样,  如果说不想返回则填写NULL,返回则需要一个地址
	pthread_exit(p+2);
}

int main(int argc, char const *argv[])
{
	//功能 : 创建一个新的线程
	//pthread_t *thread : 线程号(传出) 用于记录新的线程的线程号
	//const pthread_attr_t *attr : 线程的属性设置 填写NULL 代表系统默认的属性
	//void *(*start_routine) (void *) : 函数指针, 意味着你要该线程帮你去处理某件事情,(由用户自己编写)
										// 返回值一定是 void * 参数为void *类型的函数
	//void *arg : 传递一个参数给线程处理函数.
	// int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
    //                       void *(*start_routine) (void *), void *arg);
	//返回值: 成功 返回0  失败返回一个错误码.
	int arr[3] = {3,1};
	pthread_t tid;
	int ret = pthread_create(&tid, NULL, Fun, arr);
	if (ret != 0)
	{
		//打印对应错误码的错误原因
		printf("%s\n", strerror(ret));
		return -1;
	}

	//功能:阻塞等待线程退出
	//pthread_t thread : 你要等待那一条线程退出
	// void **retval   : 它是用于接收线程退出的返回值 (传出参数), 
						//如果说不需要接收线程的退出值则直接填写为NULL
	// 返回值 成功则为0 失败则为错误码
	void * p = NULL;
	pthread_join(tid, &p);
	printf("%d\n", *(int *)p);

	return 0;
}
