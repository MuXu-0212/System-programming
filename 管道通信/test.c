#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

//要求一直发送数据 一直读取数据
//当我们发送一个 特定的指令则需要两个进程都退出。
//并且要关闭文件描述符

//无名管道写入数据是不具有原子性.


int main(int argc, char const *argv[])
{
	//1.创建一个无名管道
	int pipefd[2];
	int ret = pipe(pipefd);
	if (ret == -1)
	{
		printf("创建无名管道失败\n");
		return -1;
	}

	//2.创建出子进程 父进程作为写端 子进程作为读端
	pid_t pid = fork();
	if (pid > 0)
	{
		char write_data[100];
		while(1)
		{
			memset(write_data, 0, sizeof(write_data));
			printf("请输入要发送的数据:\n");
			scanf("%s",write_data);
			write(pipefd[1], write_data, strlen(write_data));
			if (strcmp(write_data, "quit") == 0)
			{
				printf("等待子进程退出\n");
				wait(NULL);
				close(pipefd[0]);
				close(pipefd[1]);
				printf("子进程已退出,父进程即将退出\n");
				exit(1);
			}
		}
	}
	else if (pid == 0)
	{
		
		char read_data[100];
		while(1)
		{
			memset(read_data, 0, sizeof(read_data));
			read(pipefd[0],read_data, sizeof(read_data));
			printf("读取到的数据位:%s\n",read_data);
			if (strcmp(read_data,"quit") == 0)
			{
				close(pipefd[0]);
				close(pipefd[1]);
				printf("子进程已退出\n");
				exit(2);
			}
		}
	}
	else
	{
		printf("创建子进程失败\n");
	}
	
	return 0;
}
