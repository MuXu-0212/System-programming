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

	while(1)
	{
		//申请数据资源 P 操作 0 
		Sem_P(semid, DATA);
		printf("%s\n", shared_addr);
		//空间增加 V操作 
		Sem_V(semid, SPACE);
	}

	return 0;
}





/*

多进程
	程序 : 静态的，没有执行概念 存放磁盘当中的二进制数据
	进程 : 动态的，一个正在运行的程序, 占用CPU资源 
	进程由PCB块 数据段 代码段组成
	进程是资源管理的最小单位, 线程是系统调用的最小单位

CPU的调度策略：
	1.先到先服务 2.基于优先级 3.时间片轮询(轮转)

进程的状态：
	1.就绪态 2.执行态 3.睡眠态/挂起态 4.暂停态 5.僵尸态 6.死亡态
	僵尸态：程序运行完毕，但是PCB资源未被回收
	孤儿进程: 父进程比子进程先退出 孤儿进程由init进程进行管理回收

CPU的执行步骤： 1.预取器 读取用户指令 2.译码 把用户的二进制指令转换为CPU可识别的指令
			   3.位逻运算 + <<      4.把运算结果存放到寄存器堆当中

虚拟内存是通过物理内存映射的,用户区域是每个进程独立存在的,内核区域是共享的.

fork的特点：
	1.有两个返回值 返回值如果 大于0 代表的是父进程 等于0 则代表是子进程
	2.子进程会完全复制一份父进程的代码 父进程是从程序开始的部分进行运行
	  子进程会在fork返回值的 == 0 开始运行.
	3.子进程会完全复制一份父进程的内存资源.

fork getpid() getppid() getpgid() wait() waipid()
exec函数族 execl execlp exele 
exit() _exit()


有名管道与无名管道
pipe 会创建出两个文件描述符 0 代表读端 1 代表写端
只适用亲缘进程,
无法使用lseek去定位
数据写入时不具有原子性
无法使用open打开

mkfifo()
unlink()


信号的特点：
	1.简单 2.只能够携带少量的数据 3.需要特定的场景才能够触发

递达与未决
	递达: 信号触发并达到进程
	未决: 信号产生到递达之间的状态, 是由信号阻塞或被屏蔽产生的

可靠信号与不可靠信号
	可靠信号: 34 ~ 64 	体现在同一时间触发多次同一个信号,可以依次排队执行,信号不会丢失
	不可靠信号: 1 ~ 31  体现在同一时间内触发多次相同的信号,只会响应一次,其余的会丢失

查看信号的shell命令： kill -l

信号的三种处理方式: 1.忽略 2.捕捉 3.默认 唯独 SIGKILL 与 SIGSTOP 只允许默认
kill signal     sigqueue sigaction  pause 

信号的阻塞
sigpromask()  sigaddset sigemptyset sigdelset sigfillset sigismember

setjmp longjmp   		同一个信号响应一次之后,不再响应相同的信号
sigsetjmp siglongjmp	可选响应多次还是一次

信号的安全: 1.不允许在信号处理函数当中使用全局变量 2.信号会打断进程的睡眠态


消息队列 
查看 ipcs -q
删除 ipcrm -q 消息队列的ID  ipcrm -Q 键值

获取键值 ftok
获取消息队列的ID  msgget
发送指定数据      msgsnd
读取指定数据 	  msgrcv
删除,获取,设置消息队列    msgctl


共享内存
获取共享内存的 ID   	shmget
获取共享内存的映射地址	 shmat  取消映射 shmdt
删除,获取,设置          shmctl

信号量: 管理多进程或多线程的机制
特点:  1.信号量表示的是资源的集合,不可以为负数
	   2.如果该某个资源为0时,则会阻塞等待资源增加
	   3.它是由电气硬件所支持的
	说白了,信号量是就进程之间的全局变量,该全局变量为一个数组 每一个元素都代表着一种资源.

相关函数
获取信号量的id			semget
设置信号量的元素初始值 	 semctl 多功能函数 由cmd该参数决定功能 GETVAL SETVAL IPC_RMID IPC_STAT
申请资源或释放资源       semop  具体是P操作还是V操作需要配置结构体变量



*/