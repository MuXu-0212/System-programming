#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>

//用于接收信息结构体
struct rcv_msg
{
	long msg_flag;
	char msg_buf[50];
};

int main(int argc, char const *argv[])
{
	//申请消息队列
	int msg_id = msgget(12343, IPC_CREAT | IPC_EXCL | 0777);
	if (msg_id == -1)
	{
		if (errno == EEXIST)
		{
			msg_id = msgget(12343, 0777);
		}
		else
		{
			perror("打开消息队列失败\n");
			return -1;
		}
	}
	//存放信息结构体
	struct rcv_msg read_data;
	bzero(&read_data, sizeof(read_data));

	//读取消息队列中的数据
	msgrcv(msg_id, &read_data, sizeof(read_data), 333, 0);
	printf("读取到的消息是 : %s\n", read_data.msg_buf);

	return 0;
}


