#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <sys/msg.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>

#define  MSG_PATH "/home/gec"

struct send
{
	long send_flag;
	char recv_data[100];
};

int msgid;

int Msg_Init(void)
{
	//1.创建键值 ftok
	key_t key = ftok(MSG_PATH, 1);
	if (key == -1)
	{
		printf("创建键值失败\n");
		return -1;
	}
	
	//2.获取消息队列的ID
	msgid = msgget(key, IPC_CREAT | IPC_EXCL | 0777);
	if (msgid == -1)
	{
		if (errno == EEXIST) //ID已存在
		{
			msgid = msgget(key, 0777);
		}
		else
		{
			perror("获取消息队列ID失败\n");
			return -1;
		}
	}
	//3.返回ID
	return msgid;
}



int main(int argc, char const *argv[])
{
	Msg_Init();
	if (msgid < 0)
	{
		printf("创建消息队列失败\n");
	}
	
	// pid_t pid = fork();
	// if (pid > 0) 		//父进程
	// {
		struct send data;
		
		while(1)
		{
			memset(&data, 0, sizeof(data));
			//把自己的进程号作为 标识符
			data.send_flag = 1;
			printf("请输入你要发送的数据\n");

			fgets(data.recv_data,sizeof(data.recv_data),stdin); // 目的地 数据
			msgsnd(msgid, &data, strlen(data.recv_data), 0);
		}
	//}
	// else if (pid == 0) //子进程
	// {
	// 	struct send data;
	// 	struct msqid_ds buf;
	// 	pid_t ppid;
	// 	char recv_data[100];

	// 	while(1)
	// 	{
	// 		memset(&data, 0, sizeof(data));
	// 		msgrcv(msgid, &data, sizeof(data), 1 , 0);
	// 		printf("读取到的数据为 %s\n",data.recv_data);
	// 		sscanf(data.recv_data,"%d %s",&ppid,recv_data);
	// 		printf("ppid : %d\n", ppid);

	// 		if (strcmp(recv_data,"bye") == 0)
	// 		{
	// 			union sigval value;
	// 			value.sival_int = ppid;
	// 			sigqueue(getppid(), SIGINT, value);

	// 			printf("子进程退出\n");
	// 			exit(1);
	// 		}
	// 	}
	// }
	// else
	// {
	// 	printf("创建子进程失败\n");
	// }
	
	return 0;
}

