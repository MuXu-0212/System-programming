#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char const *argv[])
{
	//发送一个信号并且携带一个数据
	//参数 pid_t pid : 你要给哪个进程号发送信号
	//参数 int sig   : 发送什么信号
	//参数 const union sigval value
	//   union sigval {
    //            int   sival_int;   可以发送一个整型变量
    //            void *sival_ptr;   可以发送一个共享内存的地址
    //        };
	//发送成功则返回 0 失败 返回-1

	union sigval value;
	value.sival_int = 100;

	printf("%d\n",getpid());

    sigqueue(atoi(argv[1]), SIGINT, value);

	return 0;
}
