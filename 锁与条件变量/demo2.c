#include <pthread.h>
#include <unistd.h>
#include <stdio.h>


pthread_mutex_t mutex;


void handler(void *arg)
{
    printf("[%u] is end.\n",(unsigned)pthread_self());

    //解锁
    pthread_mutex_unlock((pthread_mutex_t *)arg);
}

void * thread_fun(void *arg)
{
    pthread_cleanup_push(handler,(void *)&mutex);
    pthread_mutex_lock(&mutex);

    while(1)
    {
        printf("1111111111\n");
        sleep(1);
    }

    pthread_cleanup_pop(0);
}

void *thread_fun1(void *arg)
{
    pthread_mutex_lock(&mutex);
    while(1)
    {
        printf("22222222\n");
        sleep(1);
    }
    pthread_mutex_unlock(&mutex);

}


int main()
{

    pthread_mutex_init(&mutex, NULL);

    pthread_t tid,tid1;
    int ret_p_create;
    ret_p_create = pthread_create(&tid,NULL,thread_fun,NULL);
    if(ret_p_create != 0)
    {
        printf("create thread failure\n");
        return -1;
    }

    ret_p_create = pthread_create(&tid1,NULL,thread_fun1,NULL);
    if(ret_p_create != 0)
    {
        printf("create thread failure\n");
        return -1;
    }

    sleep(2);


    pthread_cancel(tid);

    pthread_join(tid,NULL);
    pthread_join(tid1,NULL);


    pthread_mutex_destroy(&mutex);
    return 0;
}
