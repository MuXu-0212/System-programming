#include "head.h"

#define KEY_PATH "/home/gec"
#define MAXSIZE 100
struct mymsg{
	long msg_flag;
	char msg_data[MAXSIZE];
};

void* pthread_write(void* arg)
{
	char str[MAXSIZE] = {0};
	printf("请输入>");
	scanf("%s", str);
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

	struct mymsg data;
	void* ret;
	pthread_t thread_id;

	while (1)
	{
		//重置数组
		memset(&data, 0, sizeof(data));

		//创建一个子线程
		pthread_create(&thread_id, NULL, pthread_write, NULL);
		pthread_join(thread_id, &ret);
		strcpy( data.msg_data, (char*)ret);

		msgsnd(msgid, &data, strlen(data.msg_data), 0);
	}

	return 0;
}

