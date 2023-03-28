#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


void headler(int arg)
{
	printf("%d\n",arg);
}

int main(int argc, char const *argv[])
{
	
	for (size_t i = 1; i <= 64; i++)
	{
		if (i == 33 || i == 32)
		{
			continue;
		}
		
		signal(i, headler);
	}
	
	printf("%d\n",getpid());

	while(1);

	return 0;
}


// 1.信号的特点 
// 	简单 不能携带大量数据 特定条件触发
// 2.信号是由内核进行发送和处理，我们只能触发发送信号的条件

// 3.未决 ： 在发送与递达之间的状态
//   递达 ： 发送并到达

// 4.信号的可靠与不可靠与信号值有关
// 	不可靠信号 1 -  31 同一时间触发多次相同的信号,只会响应一次 其他的丢弃
// 	可靠信号   34 - 64 同一时间触发多次相同的信号,它会排队等待执行
// 	一个程序同时接收了可靠信号与不可靠信号
// 	先执行可靠信号 并从大往小执行 不可靠信号是无序执行



  	