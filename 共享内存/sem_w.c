#include "head.h"


#define SEM_PATH "/home/gec"
#define	SHM_PATH "/home/gec"
#define SPACE 0
#define DATA  1

int shmid; //共享内存的ID号
int semid;
char * shared_addr = NULL;

void Shm_Init(void)
{
	//1.获取共享内存的ID号
	shmid = shmget(ftok(SHM_PATH,123), 512, IPC_CREAT  |  IPC_EXCL | 0777);
	if (shmid == -1)
	{
		if (errno == EEXIST) //该共享内存已存在
		{
			shmid = shmget(ftok(SHM_PATH,123), 512, 0777);
		}
		else
		{
			perror("打开共享内存失败\n");
			return;
		}
	}

	//2.映射共享内存地址
	shared_addr = shmat(shmid, NULL, 0);
	if (shared_addr == (void *)-1)
	{
		perror("映射共享内存失败\n");
		return;
	}

}

int Sem_Init(void)
{
		//功能  申请信号量
	//int nsems  信号量有多少个元素,表征着你要有多少种临界资源
	semid = semget(ftok(SEM_PATH,12), 2,  IPC_CREAT  |  IPC_EXCL | 0777);
	if (semid == -1)
	{
		if (errno == EEXIST)
		{
			semid = semget(ftok(SEM_PATH,12), 2,  0777);
		}
		else
		{
			perror("创建信号量失败\n");
			return -1;
		}
	}

	//对信号量的资源进行赋值
	//多功能函数: 1.设置型号量的值 2.获取信号量的值 3.获取信号量的状态 4.删除信号量
	//该函数的功能由 CMD 参数决定 GETVAL SETVAL IPC_RMID IPC_STAT
	//int semnum :  对哪种资源进行操作
	semctl(semid, SPACE, SETVAL, 1);  //对空间的值进行初始化
	semctl(semid, DATA, SETVAL, 0);   //对数据进行赋值;


	// int ret = semctl(semid, SPACE, GETVAL);
	// printf("SPACE %d\n", ret);
	// ret = semctl(semid, DATA, GETVAL);
	// printf("DATA %d\n", ret);

	return semid;
}



//对信号量数组semid中的number个下标进行P操作,P操作 资源-1;
void Sem_P(int semid, int number)
{
    struct sembuf P;
    P.sem_num = number;     //表示要操作该数组中的那个元素下标
    P.sem_op  = -1;         //具体的操作数值.可以不为1
    P.sem_flg = 0;

    //执行一次P操作
    semop(semid,&P,1);
}

//对信号量数组semid中的number个下标进行V操作,V操作 资源+1;
void Sem_V(int semid, int number)
{
	
	//struct sembuf *sops : 一个结构体变量 该结构体变量需要用户自己配置
		//unsigned short sem_num; : 表示你要操作的资源的下标
		//short          sem_op;  : 表示你到底要进行 P操作还是V操作
		//short          sem_flg  : SEM_UNDO 相当于复位 填写为0 则是保留上次退出状态
	//size_t nsops        :  你要操作几次
    struct sembuf V;
    V.sem_num = number;     //表示要操作该数组中的那个元素下标
    V.sem_op  = 1;          //具体的操作数值.可以不为1
    V.sem_flg = 0;

    //执行一次V操作
    semop(semid,&V,1);
}
int main(int argc, char const *argv[])
{

	Shm_Init();
	Sem_Init();

	//P(减法) V(加法)

	struct sembuf sops;
	while(1)
	{
		//申请内存 P 操作 0 
		Sem_P(semid, SPACE);
		scanf("%s", shared_addr);
		//数据增加 V操作 
		Sem_V(semid, DATA);
	}

	return 0;
}
