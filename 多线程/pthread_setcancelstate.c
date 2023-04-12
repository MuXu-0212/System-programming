#include "head.h"

void *Fun(void *arg)
{
	//取消状态：可取消与不可取消
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, 	NULL);
	//取消类型: 延迟取消 与 立即取消
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	while(1)
	{
		printf("11111111\n");
		sleep(1);
	}
}

int main(int argc, char const *argv[])
{	

	pthread_t tid;
	pthread_create(&tid, NULL ,Fun, NULL);
	sleep(3);
	pthread_cancel(tid);

	pause();

	return 0;
}
