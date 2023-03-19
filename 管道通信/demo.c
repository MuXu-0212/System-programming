#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>



int main(int argc, char const *argv[])
{
	//功能: 创建一个无名管道
	//参数 int pipefd[2]: 存放2个无名管道的文件描述符 pipefd[0]为读端 pipefd[1]为写端
	//返回值 成功为 0  失败为 -1
	//int pipe(int pipefd[2]);
	int fd[2];
	int ret = pipe(fd);
	if (ret == -1)
	{
		printf("创建无名管道失败\n");
		return -1;
	}
	pid_t pid = fork();
	if (pid > 0)   //作为写端
	{
		printf("请输入你要发送的数据:\n");
		char write_data[100];
		scanf("%s",write_data);
		write(fd[1], write_data, strlen(write_data));

		wait(NULL);
	}
	else if(pid == 0) //作为读端
	{
		char read_data[100] = {0};
		read(fd[0],read_data,sizeof(read_data));
		printf("接收到父进程的信息为:%s\n",read_data);
	}
	else
	{
		printf("创建失败\n");
	}

	close(fd[0]);
	close(fd[1]);

	return 0;
}


