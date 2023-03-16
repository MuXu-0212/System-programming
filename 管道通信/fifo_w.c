#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>


#define PATH_FIFO "/home/gec/fifo"

int Fifo_Init()
{
	int fd_fifo;
	int ret = access(PATH_FIFO, F_OK);
	if (ret == 0)
	{
		fd_fifo = open(PATH_FIFO, O_RDWR);
		if (fd_fifo == -1)
		{
			printf("打开管道文件失败!\n");
			return -1;
		}
	}
	else
	{
		ret = mkfifo(PATH_FIFO, 0777);
		if (ret == -1)
		{
			printf("创建管道文件失败\n");
			return -2;
		}

		fd_fifo = open(PATH_FIFO, O_RDWR);
		if (fd_fifo == -1)
		{
			printf("打开管道文件失败\n");
			return -1;
		}
	}
	return fd_fifo;
}

int main(int argc, char const *argv[])
{
	int fd_fifo = Fifo_Init();
	char write_message[100] = {0};

	while (1)
	{
		memset(write_message, 0, sizeof(write_message));
		printf("请输入一个数据>");
		scanf("%s", write_message);
		write(fd_fifo, write_message, sizeof(write_message));
		if (strcmp(write_message, "exit") == 0)
		{
			break;
		}
	}
	// printf("写入的数据为 ： %s\n", get_message);

	close(fd_fifo);
	unlink(PATH_FIFO);
	return 0;
}