#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>


#define KEY_PATH "/home/gec"

struct mymsg{
	long flag;
	char data[100];
};

int msgid;

int main(int argc, char const *argv[])
{
	/*
		1.想要拥有键值
		ftok()
		功能:创建一个键值, 不同的路径下可以有相同的键值
		const char* pathname : 创建键值的路径
		int proj_id : 随便一个int类型数据(标识符)
		返回值 则返回一个键值 失败返回-1
	*/
	key_t key = ftok(KEY_PATH, 1);
	if (key == -1)
	{
		printf("创建键值失败\n");
		return -1;
	}

	/*
		msgget()
		功能:获取ID号
		key_t key : 需要一个键值
		int msgflg : IPC_CREAT 不存在则创建 TPC_EXCL存在则报错
		返回值 成功则返回消息队列的ID号 失败则返回 -1
	*/

	int msgid = msgget(key, IPC_CREAT | IPC_EXCL | 0777);
	if (msgid == -1)
	{
		if (errno == EEXIST)
		{
			printf("打开消息队列成功\n");
			msgid = msgget(key, 0777);
		}
		else
		{
			printf("获取消息队列的ID失败\n");
		}
	}
	printf("键值为%x\n", key);
	printf("消息队列的ID号为 : %d\n", msgid);

	/*
		进行通信
		功能发送数据;
		int msqid : 共享内存的ID号
		const void* msgp : 你要发送的数据
		size_t msgsz : 数据的大小
		int msgflg : 默认为0
	*/

	struct mymsg data;
	memset(&data, 0, sizeof(data));
	data.flag = 1;
	strcpy(data.data, "hello");
	msgsnd(msgid, &data, strlen(data.data), 0);

	memset(&data, 0, sizeof(data));
	data.flag = 2;
	strcpy(data.data, "world");
	msgsnd(msgid, &data, strlen(data.data), 0);

	memset(&data, 0, sizeof(data));
	data.flag = 3;
	strcpy(data.data, "hello world");
	msgsnd(msgid, &data, strlen(data.data), 0);
	return 0;
}

