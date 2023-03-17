#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


/*
	捕捉一个信号
*/


void signal_handling(int sig)
{
	printf("我收到 %d 号信号\n", sig);
}

int main(int argc, char const *argv[])
{
	signal(SIGINT, signal_handling);
	while(1)
	{
		printf("1111\n");
		sleep(1);
	}
	return 0;
}
