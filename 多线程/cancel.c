#include "head.h"

void *Fun(void *arg)
{
	while(1)
	{
		printf("线程当中获取：%ld\n",pthread_self());
		sleep(1);
	}
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

	sleep(3);
	//功能:强制取消线程的运行状态
	//参数： 你要取消哪一条线程
	//返回值 成功为 0  失败 错误码
	pthread_cancel(tid);


	printf("主线程当中的子线程号 ：%ld\n",tid);
	ret = pthread_join(tid, NULL);
	if(ret != 0)
	{
		//打印对应错误码的错误原因
		printf("%s\n", strerror(ret));
		return -1;
	}

	return 0;
}