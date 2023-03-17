#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

void handle_sig(int sig)
{
	printf("收到信号:%d\n", sig);
}

int main(int argc, char const *argv[])
{
	//捕捉信号
	signal(SIGALRM, handle_sig);
	alarm(5);
	pause();

	return 0;
}

