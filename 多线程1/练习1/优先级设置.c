#include "head.h"

void *thread_1(void * arg)
{
    int i,j;
    while (1)
    {
		fprintf(stderr,"%c",*(char *)arg);
    	for (i = 0; i < 100000; i++)
        for(j = 0;j < 1000; j++);
    }
}
void *thread_2(void * arg)
{
    int i,j;
    while (1)
    {
		fprintf(stderr,"%c",*(char *)arg);
    	for (i = 0; i < 100000; i++)
        for(j = 0;j < 1000; j++);
    }
}
void *thread_3(void * arg)
{
    int i,j;
    while (1)
    {
		fprintf(stderr,"%c",*(char *)arg);
    	for (i = 0; i < 100000; i++)
        for(j = 0;j < 1000; j++);
    }
}
 
int main(int arg,char ** argv)
{
    int res,rc1,rc2,rc3;
    pthread_t thread1,thread2,thread3;
    long t=100;
    long result2;
    pthread_attr_t attr1,attr2,attr3;//未初始化，attr里面的值为空
    struct sched_param param1,param2;
    pthread_attr_init(&attr1);//初始化，attr的值为默认值
    pthread_attr_init(&attr2);
    pthread_attr_setinheritsched(&attr1,PTHREAD_EXPLICIT_SCHED);//不继承，只有不继承父线程的调度策略才可以设置线程的调度策略
    pthread_attr_setinheritsched(&attr2,PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&attr1,SCHED_RR);//抢占式调度策略
    pthread_attr_setschedpolicy(&attr2,SCHED_RR);//抢占式调度策略

    param1.sched_priority=1;
    param2.sched_priority=20;

    pthread_attr_setschedparam(&attr1,&param1);
    pthread_attr_setschedparam(&attr2,&param2);
    //pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);//设置线程分离态，pthread_join()失效
    rc1 = pthread_create( &thread1,&attr1,thread_1,(void *)"1");
    rc2 = pthread_create( &thread2,&attr2,thread_2,(void *)"2");
    rc3 = pthread_create( &thread3,NULL,thread_3,(void *)"3");
    pthread_attr_destroy(&attr1);//删除属性
    pthread_attr_destroy(&attr2);
    pthread_join(thread1,NULL);
    pthread_join(thread2,NULL);
    pthread_join(thread3,NULL);
}