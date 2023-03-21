#include "head.h"

/*
	example 2:
	使用共享内存进行通信
	example 2接收example 1的信息
*/

#define PATH_KEY "/home/gec"

int main(int argc, char const *argv[])
{
	//1.准备键值
	key_t key = ftok(PATH_KEY, 8);
	if (key == -1)
	{
		perror("Application for key failed.\n");
		return -1;
	}

	//2.申请共享内存
	int shm_id = shmget(key, 512, IPC_CREAT | IPC_EXCL | 0777);
	if (shm_id == -1)
	{
		if (errno == EEXIST)
		{
			shm_id = shmget(key, 512, 0777);
		}
		else
		{
			perror("Failed to apply shared memory.\n");
			return -2;
		}
	}

	//3.映射获取首地址
	char* shm_address = shmat(shm_id, NULL, 0);
	if (shm_address == NULL)
	{
		perror("Failed\n");
		return -3;
	}

	//接收example 1发送过的数据
	while (1)
	{
		if (strcmp(shm_address, "over") == 0)
		{
			break;
		}
		printf("接收example 1的数据为：%s\n", shm_address);
	}


	return 0;
}