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

struct mymsg 
 {
    long   msg_flag;         /* Message type. */
    char   msg_data[100];    /* Message text. */
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
	struct mymsg data;
	int temp; 		//要把数据转发给谁
	char buf[100]; 	//要转发的数据
	while(1)
	{
		memset(&data, 0, sizeof(data));
		msgrcv(msgid, &data, sizeof(data), 1 , 0);
		printf("读取到的数据为 %s\n",data.msg_data);

		//把读取到的数据进行拆分 然后进行转发
		sscanf(data.msg_data,"%d %s",&temp, buf);

		data.msg_flag = temp;
		msgsnd(msgid, &data, strlen(data.msg_data), 0);
	}

	return 0;
}
