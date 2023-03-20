#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>

/*
	int msgget(key_t key, int msgflg);
	参数解析:
	参数1 key_t key : 消息队列的键值
	参数2 msgflg    :
		IPC_CREAT : 如果key对应的MSG不存在, 则创建MSG
		IPC_EXCL : 如果key对应的MSG存在, 则报错
		mode  :  MSG的访问权限(八进制权限, 例如 0666)
	返回值  :  
		成功 :　返回该消息队列的ID号
		失败 : 返回-1
*/

struct send_msg
{
	long msg_flag;
	char msg_buf[50];
};

int main(int argc, char const *argv[])
{
	int msg_id = msgget(123456, IPC_CREAT | IPC_EXCL | 0777);
	if (msg_id == -1)
	{
		if (errno == EEXIST)
		{
			msg_id = msgget(123456, 0777);
		}
		else 
		{
			perror("打开消息队列失败\n");
			return -1;
		}
	}
	//定义一个结构体变量存放要发送的数据
	struct send_msg data;
	bzero(&data, sizeof(data)); //清空结构体
	data.msg_flag = 111;
	strcpy(data.msg_buf, "hello");

	//定义一个结构体变量存放要发送的数据
	struct send_msg data1;
	bzero(&data1, sizeof(data1));
	data1.msg_flag = 222;
	strcpy(data1.msg_buf, "123");

	//定义一个结构体变量存放要发送的数据
	struct send_msg data2;
	bzero(&data2.msg_buf, sizeof(data2));

	data2.msg_flag = 333;
	strcpy(data.msg_buf, "gec");

	//发送消息
	int send_ret = msgsnd(msg_id, &data, strlen(data), 0);
	if (send_ret == -1)
	{
		printf("send data faile\n");
		return -1;
	}
	send_ret = msgsnd(msg_id, &data1, strlen(data1), 0);
	if (send_ret == -1)
	{
		printf("send data1 faile\n");
		return -2;
	}
	send_ret = msgsnd(msg_id, &data2, strlen(data2), 0);
	if (send_ret == -1)
	{
		printf("send data2 faile\n");
		return -3;
	}


	return 0;
}


