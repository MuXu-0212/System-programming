#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void Fun1(int arg)
{
	printf("收到了 %d 号信号\n",arg);
}



void Fun(int arg, siginfo_t *inof, void *a)
{
	printf("收到了 %d 号信号\n",inof->si_signo);
	printf("%d 发送的信号\n",inof->si_pid);
	printf("收到的额外数据是 %d\n",inof->si_int);
}

int main(int argc, char const *argv[])
{
	printf("%d\n",getpid());
	//处理一个信号 
	//参数 int sig： 你要处理哪一个信号
	//const struct sigaction *act, 接收信号的方式 (用户自己配置)
	//struct sigaction *oldact   , 旧的接收信号方式
	struct sigaction act;
	act.sa_sigaction = Fun1;
	act.sa_flags = SA_SIGINFO;
	sigaction(SIGINT, &act, NULL);

	while(1);
	return 0;
}
