#include "head.h"

void *Fun(void *arg)
{

	printf("线程当中获取：%ld\n",pthread_self());

	// int a = 10,
	//     b = 20;
	//static int c;
	//c = a + b;
	//pthread_exit(&c);

	// int *p = (int *)malloc(sizeof(int));
	// if (p == NULL)
	// {
	// 	printf("申请堆空间失败\n");
	// 	return NULL;
	// }
	// *p = a + b;
	// pthread_exit(p);
}

int main(int argc, char const *argv[])
{
	pthread_t tid;
	int ret = pthread_create(&tid, NULL, Fun, NULL);
	if (ret != 0)
	{
		//打印对应错误码的错误原因
		printf("%s\n", strerror(ret));
		return -1;
	}

	printf("主线程当中的子线程号 ：%ld\n",tid);
	pthread_join(tid, NULL);
	
	//free(p);
	
	return 0;
}
