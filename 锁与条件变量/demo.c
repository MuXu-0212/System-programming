#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include <errno.h>
#include <pthread.h>

//定义互斥锁变量
pthread_mutex_t m;

void output(const char *msg)
{
    while(*msg != '\0')
    {
        putc(*msg, stdout);  //不缓冲单个字符输出到屏幕.
        usleep(100);
        msg += 1;
    }
    return;
}

void *routine(void *arg)
{
    pthread_mutex_lock(&m);     //加锁
    output("info output by sub-thread.\n");
    pthread_mutex_unlock(&m);   //解锁
    pthread_exit(NULL);
}

int main(void)
{
    // 初始化
    pthread_mutex_init(&m, NULL);/* 定义一个标准的互斥锁 */

    //创建线程
    pthread_t tid;
    pthread_create(&tid, NULL, routine, NULL);
 
    // 在进入临界区之前，加锁
    pthread_mutex_lock(&m);
    // 访问了共享资源（临界资源）的代码，称为临界区代码
    output("message delivered by main thread.\n");
    // 在离开临界区的时候，解锁
    pthread_mutex_unlock(&m);
 
    //等待线程退出
    pthread_join(tid, NULL); 
    //线程退出后 销毁锁
    pthread_mutex_destroy(&m);
    pthread_exit(NULL);
}