#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <fcntl.h>



int main(int argc, char const *argv[])
{
	int fd_pipe[2];
	int ret = pipe(fd_pipe);

	if (ret == -1)
	{
		printf("创建管道文件失败!\n");
		return -1;
	}

	pid_t pid = fork();
	if ( pid > 0)
	{
		char write_data[50];
		while (1)
		{
			sleep(0.2);
			memset(write_data, 0, sizeof(write_data));
			printf("请输入数据>");
			scanf("%s", write_data);
			write(fd_pipe[1], write_data, strlen(write_data));
		}
	}
	else if (pid == 0)
	{
		char read_data[50];
		while (1)
		{
			// sleep(0.1);
			memset(read_data, 0, sizeof(read_data));
			read(fd_pipe[0], read_data, sizeof(read_data));
			printf("接收到的数据>%s\n", read_data);
			// sleep(1);
		}
	}
	else
	{
		printf("打开管道文件失败!\n");
		return 1;
	}

	return 0;
}