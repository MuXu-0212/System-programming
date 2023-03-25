#include "head.h"


#define KEY_PATH "/home/gec"


int main(int argc, char const *argv[])
{
	//函数功能：申请共享内存(获取共享内存的ID号)
	//key_t key   : 需要一个键值
	//size_t size : 共享内存的大小  一定要是512字节的倍数
	//int shmflg  : 八进制的权限值 + 可选项 IPC_CREAT IPC_EXCL
	//返回值: 成功则返回一个共享内存的ID  失败 -1
	int shmid = shmget(ftok(KEY_PATH,123), 512, IPC_CREAT | IPC_EXCL | 0777);
	if (shmid == -1)
	{
		if (errno == EEXIST) //该共享内存已存在
		{
			shmid = shmget(ftok(KEY_PATH,123), 512, 0777);
		}
		else
		{
			perror("打开共享内存失败\n");
			return -1;
		}
	}

	//函数功能: 映射共享内存,获取到共享内存的地址
	//int shmid 		  : 你要映射哪一个共享内存
	//const void *shmaddr : 一般填写为NULL, 让操作系统帮你找一块合适的内存空间
	// int shmflg : 以什么样的方式映射共享内存 填写为 0 则意味着可读可写 SHM_RDONLY 代表为只读
	//成功返回一个共享内存的地址  失败 则返回 (void *)-1
	char *shared_addr = shmat(shmid, NULL, 0);
	if (shared_addr == (void *)-1)
	{
		perror("获取共享内存地址失败\n");
		return -2;
	}
	
	//读写数据
	while(1)
	{
		printf("%s\n",shared_addr);
		if (strcmp(shared_addr,"bye") == 0)
		{
			break;
		}
	}
	//解除共享内存的映射
	shmdt(shared_addr);

	//IPC_STAT IPC_SET IPC_RMID
	shmctl(shmid, IPC_RMID, NULL);



	return 0;
}