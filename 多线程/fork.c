#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
	//创建一个子进程杀毒，父进程清理垃圾
	pid_t ret = fork();
	if (ret > 0)
	{
		while (1)
		{
			printf("")
			printf("父进程正在清理病毒!\n");
			sleep(1);
		}
	}
	else if (ret == 0)
	{
		while (1)
		{
			printf("子进程正在清理垃圾\n");
			sleep(1);
		}
	}
	else
	{
		printf("创建子进程失败！\n");
	}
	return 0;
}