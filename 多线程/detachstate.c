#include "head.h"

void *Fun(void *arg)
{
	int i = 0;
	while(1)
	{
		printf("%d\n",i);
		sleep(1);
		i++;
		if (i == 20)
		{
			break;
		}
	}
}

void *Fun1(void *arg)
{
	//主动分离
	pthread_detach(pthread_self());
	int i = 0;
	while(1)
	{
		printf("%d\n",i);
		sleep(1);
		i++;
		if (i == 20)
		{
			break;
		}
	}
}

int main(int argc, char const *argv[])
{	
	// pthread_attr_t attr;
	// //1.要对这个变量进程初始化
	// pthread_attr_init(&attr);
	// //2.配置该变量的属性
	// pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	// pthread_t tid;
	// //3.把配置好的属性直接丢到 create 函数当中 (被迫分离)
	// pthread_create(&tid, &attr ,Fun, NULL);

	//4.销毁之前配置的属性
	//pthread_attr_destroy(&attr);

	pthread_t tid;
	pthread_create(&tid, NULL ,Fun1, NULL);
	
	pause();
	
	return 0;
}
