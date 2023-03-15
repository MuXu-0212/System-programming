
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>


//创建3个以上的进程. 3个进程要同时进行不同的任务 1.杀毒 2.扫描漏洞 3.清理垃圾 每个要打印5句
//有次序的退出(父进程不能够比子进程先退出).

int main(int argc, char const *argv[])
{
	int i = 5;
	int ret_value;
	pid_t pid = fork();

	if (pid > 0)
	{
		while (i--)
		{
			printf("正在杀毒！\n");
			sleep(1);
		}

		//阻塞等待子进程结束
		waitpid(pid, &ret_value, 0);
		if (WIFEXITED(ret_value))
		{
			printf("退出值为：%d\n", WEXITSTATUS(ret_value));
		}
	}
	else if (pid == 0)
	{
		pid_t child_pid = fork();
		if (child_pid > 0)
		{
			while (i--)
			{
				printf("正在扫描漏洞\n");
				sleep(1);
			}
			waitpid(child_pid, &ret_value, 0);
			if (WIFEXITED(ret_value))
			{
				printf("退出值为：%d\n", WEXITSTATUS(ret_value));
			}
			printf("儿子进程退出\n");
			exit(2);
		}
		else if (child_pid == 0)
		{
			while (i--)
			{
				printf("正在清理垃圾\n");
				sleep(1);
			}
			printf("孙子进程退出\n");
			exit(1);
		}
	}
	else
	{
		printf("创建子进程失败\n");
	}
	printf("祖父进程准备退出\n");
	return 0;
}