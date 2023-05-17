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

void Fun(int arg, siginfo_t *info, void *a)
{
	//杀死对方
	kill(info->si_int, SIGKILL);
	
	//销毁消息队列
	msgctl(msgid, IPC_RMID, NULL);
	//自杀
	kill(getpid(),SIGKILL);
}

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
	msgid = Msg_Init();
	if (msgid < 0)
	{
		printf("创建消息队列失败\n");
	}
	
	pid_t pid = fork();
	if (pid > 0) 		//父进程
	{
		//配置信号接收结构体
		struct sigaction data1;
		data1.sa_sigaction = Fun;
		data1.sa_flags = SA_SIGINFO;
		sigaction(SIGINT, &data1, NULL);

		struct send data;
		char buf[100];
		while(1)
		{
			memset(&data, 0, sizeof(data));
			
			//把自己的进程号作为 标识符
			data.send_flag = 1;
			printf("请输入你要发送的数据\n");
			scanf("%s",buf);
			sprintf(data.recv_data,"%d %s",getpid(),buf);
			msgsnd(msgid, &data, strlen(data.recv_data), 0);
			if (strcmp(buf, "bye") == 0)
			{
				data.send_flag = 2;
				sprintf(data.recv_data,"%d bye",getpid());
				msgsnd(msgid, &data, strlen(data.recv_data), 0);
				wait(NULL);
			}
		}
	}
	else if (pid == 0) //子进程
	{
		struct send data;
		struct msqid_ds buf;
		pid_t pid,ppid;
		char recv_data[100];

		while(1)
		{
			memset(&data, 0, sizeof(data));

			msgctl(msgid, IPC_STAT, &buf);
			msgrcv(msgid, &data, sizeof(data), 2 , 0);
			printf("读取到的数据为 %s\n",data.recv_data);
			sscanf(data.recv_data,"%d %s",&ppid,recv_data);
			printf("ppid : %d\n", ppid);

			if (strcmp(recv_data,"bye") == 0)
			{
				union sigval value;
				value.sival_int = ppid;
				sigqueue(getppid(), SIGINT, value);

				printf("子进程退出\n");
				exit(1);
			}
		}
	}
	else
	{
		printf("创建子进程失败\n");
	}
	
	

	return 0;
}

