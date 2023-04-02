
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <pthread.h>

//初始化一把锁变量
pthread_mutex_t lock;

void *fun(void * arg) //线程任务函数   接口为固定的除了函数名 与 参数名可以改变,其他的不允许改变
{
    int num = '0';
	pthread_mutex_lock(&lock); 
    for (int i = 0; i < 10; ++i)
    {
        fprintf(stderr,"%c", num + i ); //不缓冲方式输出到屏幕
        usleep(10*1000);
    }
	//解锁
	pthread_mutex_unlock(&lock);
}

//互斥锁 确保同一时间内只有一条线程能够访问临界资源

int main(int argc, char const *argv[])  //执行main函数的线程 被称为主线程
{
    
	//功能：初始化一把互斥锁
	//pthread_mutex_t  *mutex : 你要初始化哪一把锁
	//const   pthread_mutexattr_t*mutexattr :  该锁的属性需不需要设置， 不需要则直接填NULL;
	//成功则返回 0 失败则返回错误码
	pthread_mutex_init(&lock, NULL);

    pthread_t thread_id;
    //1.创建一个子线程
    pthread_create(&thread_id, NULL, fun,  NULL); 

	//上锁
	pthread_mutex_lock(&lock);
    char ch = 'a';
    for (int i = 0; i < 26 ; ++i)
    {
        fprintf(stderr,"%c", ch+i);  //不缓冲方式输出到屏幕
        usleep(10*1000);
    }
	//解锁
	pthread_mutex_unlock(&lock);
    //等待以上线程退出,并获取退出值
    pthread_join(thread_id, NULL);
	pthread_mutex_destroy(&lock);

    return 0;
}