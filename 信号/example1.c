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

	signal(SIGCONT, headler3);

	int fifo_fd = Fifo_Init();
	pid_t pid = getpid();
	//把自己的pid号写入到管道当中去
	write(fifo_fd, &pid, sizeof(pid));

	

	while(1)
	{
		printf("%d\n", getpid());
		sleep(1);
	}

	return 0;
}
