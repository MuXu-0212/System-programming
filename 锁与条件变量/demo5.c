#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <errno.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#include <pthread.h>
#include <semaphore.h>

// 定义一个POSIX无名信号量
sem_t s; 

void *routine(void *arg)
{
    char *buf = (char *)arg;

    // 计算buf中的字符串的长度
    while(1)
    {
        // 静静等待对方的数据准备好。。。
        // P操作
        sem_wait(&s); //1 0
        printf("%lu\n", strlen(buf));
    }
}

int main(int argc, char **argv)
{
    char buf[100];
    sem_init(&s, 0/* 零代表线程间使用，非零是进程间使用*/, 0/*初始值*/);

    pthread_t tid;
    pthread_create(&tid, NULL, routine, (void *)buf);

    // 不断输入数据到buf
    while(1)
    {
        fgets(buf, 100, stdin);
        // 使用信号量通知对方可以开始计算了
        // V操作
        sem_post(&s);  //0 1
    }

    return 0;
}