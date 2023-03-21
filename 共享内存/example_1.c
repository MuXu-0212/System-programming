#include "head.h"


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
		if (errno == EEXIST)  //说明共享内存已经申请
		{
			//打开共享内存
			shm_id = shmget(key, 512, 0777);
		}
		else
		{
			perror("Failed to apply for shared memory.\n");
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

/*
	4.通过example 1发送信息给example 2的信息
	可以直接写入数据到映射空间内，
	因为我们之前已经通过shmget函数申请了512字节的空间
*/
	while (1)
	{
		printf("Please enter the data to be sent >\n");
		scanf("%s", shm_address);
		if (strcmp(shm_address, "over") == 0)
		{
			break;
		}
	}
	//shamdt(shm_address);

	//解除共享内存的映射
	shmdt(shared_address);

	shmctl(shm_id, IPC_RMID, NULL);

	return 0;
}