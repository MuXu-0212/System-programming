#include "head.h"


pthread_cond_t  cond;    //定义一个条件变量
pthread_mutex_t mutex;   //定义一个互斥锁

int count = 0;  //代表的钱

void diedie(int arg)
{
	//上锁
	pthread_mutex_lock(&mutex);
	//打钱
	count += 100;
	//printf("钱已到账\n");
	//通知小明钱已经打过去(唤醒线程)
	pthread_cond_signal(&cond);
	//pthread_cond_broadcast(&cond);
	//解锁
	pthread_mutex_unlock(&mutex);
}

void *xiaoming(void *arg)
{
	int i = 3;
	while(i--)
	{
		//上锁
		pthread_mutex_lock(&mutex);
		//等待钱的到来
		while(count < 13)
		{
			//1.阻塞线程
			//2.解锁
			//3.进入睡眠状态
			printf("小明说：爹爹快打钱\n");
			pthread_cond_wait(&cond, &mutex);
		}
		//当满足以上条件的时候.则是需要付钱走人.
		sleep(1);
		count -= 13;
		printf("小明账户上还有 %d \n",count);
		//解锁
		pthread_mutex_unlock(&mutex);
	}
}

void *meimei(void *arg)
{
	int i = 3;
	while(i--)
	{	//上锁
		pthread_mutex_lock(&mutex);
		//等待钱的到来
		while(count < 13)
		{
			//1.阻塞线程
			//2.解锁
			//3.进入睡眠状态
			printf("妹妹说：爹爹快打钱\n");
			pthread_cond_wait(&cond, &mutex);
		}
		//当满足以上条件的时候.则是需要付钱走人.
		sleep(1);
		count -= 13;
		printf("妹妹账户上还有 %d \n",count);
		//解锁
		pthread_mutex_unlock(&mutex);
	}
}



int main(int argc, char const *argv[])
{
	
	signal(SIGINT, diedie);

	//初始化互斥锁以及条件变量
	pthread_cond_init(&cond, NULL);
	pthread_mutex_init(&mutex,NULL);

	//创建出一条线程
	pthread_t tid,tid1;
	pthread_create(&tid, NULL, xiaoming, NULL);
	pthread_create(&tid1, NULL, meimei, NULL);
	//等待线程退出
	pthread_join(tid,NULL);

	//销毁条件变量及互斥锁
	pthread_cond_destroy(&cond);
	pthread_mutex_destroy(&mutex);

	while(1);

	return 0;
}
