#include "head.h"

#define KEY_PATH "/home/gec"
#define MAXSIZE 100

struct mymsg{
	long msg_flag;
	char msg_data[MAXSIZE];
};

void* pthread_read(void* arg)
{
	char* str = (char*)arg;
	pthread_exit(str);
}

int main(int argc, char const *argv[])
{
	//获取键值
	key_t key = ftok(KEY_PATH, 1);
	if (key == -1)
	{
		printf("键值创建失败\n");
		return -1;
	}

	//获取ID号
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

	pthread_t thread_id;
	struct mymsg data;
	void* ret;

	while (1)
	{
		//重置数组
		memset(&data, 0, sizeof(data));


		msgrcv(msgid, &data, sizeof(data.msg_data), 0, 0);

		//创建一个子线程
		pthread_create(&thread_id, NULL, pthread_read, data.msg_data);
		
		//等待以上线程退出,并获取退出值
		pthread_join(thread_id, &ret);

		printf("%s\n", (char*)ret);
	}
}