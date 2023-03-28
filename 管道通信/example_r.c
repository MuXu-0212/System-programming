#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define FIFO_PATH "/home/gec/fifo"


int Fifo_Init(void)
{
	int fifo_fd;
	int ret = access(FIFO_PATH, F_OK);
	if (ret == 0)
	{
		//直接打开管道文件
		fifo_fd = open(FIFO_PATH, O_RDWR);
		if (fifo_fd == -1)
		{
			printf("打开管道文件失败\n");
			return -1;
		}
	}
	else
	{
		//创建并打开管道文件
		//功能：创建一个有名管道
		//const char *pathname : 管道的路径(纯linux环境才能够使用)
		//mode_t mode : 八进制权限 0777
		//返回值为 成功返回0 失败返回 -1
		ret = mkfifo(FIFO_PATH, 0777);
		if (ret == -1)
		{
			printf("创建管道文件失败\n");
			return -2;
		}
		//直接打开管道文件
		fifo_fd = open(FIFO_PATH, O_RDWR);
		if (fifo_fd == -1)
		{
			printf("打开管道文件失败\n");
			return -1;
		}
	}

	return fifo_fd;
}


int main(int argc, char const *argv[])
{
	
	int fifo_fd = Fifo_Init();
	//对管道进行操作
	char recv_msg[100] = {0};
	
	read(fifo_fd, recv_msg, sizeof(recv_msg));
	printf("读取到的数据为 :%s\n",recv_msg);

	close(fifo_fd);
	unlink(FIFO_PATH);

	return 0;
}

