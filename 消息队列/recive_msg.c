#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>


#define PATH_KEY "/home/gec"
#define MAXSIZE 100

struct mymsg{
	long flag;
	char data[MAXSIZE];
};

int main(int argc, char const *argv[])
{
	/*
		1.先要拥有键值
		ftok()
		功能 : 创建一个键值,不同的路径有相同的键值
		const char* pathname : 创建键值的路径
		int proj_id : 随便一个int类型数据(标识符)
		返回值 成功则返回一个键值 失败返回-1
	*/

	key_t key = ftok(PATH_KEY, 1);
	if (key == -1)
	{
		printf("创建键值失败!\n");
		return -1;
	}

	int msgid = msgget(key, IPC_CREAT | IPC_EXCL | 0777);
	if (msgid == -1)
	{
		if (errno == EEXIST)
		{
			printf("打开消息队列成功!\n");
		}
		else
		{
			printf("获取消息队列的ID失败\n");
		}
	}
	printf("键值为:%x\n", key);
	printf("消息队列的ID号为 : %d\n", msgid);



	struct mymsg recv_data;
	memset(&recv_data, 0, sizeof(recv_data));
	msgrcv(msgid, &recv_data, sizeof(recv_data), 3, 0);
	printf("%s\n", recv_data.data);

	return 0;
}


