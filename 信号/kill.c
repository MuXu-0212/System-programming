
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#define PATH_FIFO "/home/gec/fifo"

// int fifo_init(void)
// {
// 	if (access(PATH_FIFO, F_OK) != 0)
// 	{
// 		mkfifo(PATH_FIFO, 0777);
// 	}

// 	int fd = open(PATH_FIFO, O_RDWR);
// 	return fd;
// }

int main(int argc, char const *argv[])
{

	while (1)
	{
		printf("进程号:%d\n", getpid());
		sleep(1);
	}


	return 0;
}

