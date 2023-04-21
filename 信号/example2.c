#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#define FIFO_PATH "/home/gec/fifo"

int Fifo_Init(void)
{
	int ret = access(FIFO_PATH, F_OK);
	if (ret != 0)
	{
		mkfifo(FIFO_PATH, 0777);
	}
	int fd = open(FIFO_PATH, O_RDWR);
	return fd;
}

int main(int argc, char const *argv[])
{

	int fifo_fd = Fifo_Init();
	
	//从管道当中去读取数据然后发送信号
	pid_t pid;
	read(fifo_fd, &pid, sizeof(pid));

	sleep(3);
	kill(pid, SIGSTOP);
	sleep(3);
	
	kill(pid, SIGCONT);
	sleep(3);

	kill(pid, SIGKILL);

	close(fifo_fd);

	unlink(FIFO_PATH);
	
	return 0;
}
