#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>
#define KEY_PATH "/home/gec"

struct mymsg 
 {
    long   msg_flag;         /* Message type. */
    char   msg_data[100];    /* Message text. */
};


int main(int argc, char const *argv[])
{
	//1.先要拥有键值
	//功能:创建一个键值,不同的路径下可以有相同的键值
	//const char *pathname : 创建键值的路径
	//int proj_id : 随便一个int类型数据 (标识符)
	//返回值 则返回一个键值 失败返回 -1
	key_t key = ftok(KEY_PATH, 1);
	if (key == -1)
	{
		printf("创建键值失败\n");
		return -1;
	}
	//功能：获取ID号
	//key_t key : 需要一个键值
	//int msgflg:  IPC_CREAT 不存在则创建  IPC_EXCL 存在则报错
	//返回值 成功则返回消息队列的ID号 失败则返回 -1
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
	printf("键值为:%x\n",key);
	printf("消息队列的ID号为 ：%d\n",msgid);


	//功能：从消息队列里面读取指定的数据
	//int msqid 消息队列的ID
	// void *msgp   数据存放的地方
	// size_t msgsz 数据的大小
	// long msgtyp  你要读取数据的标识符(重点)
	// int msgflg   默认设置为0
	struct mymsg recv_data;
	memset(&recv_data, 0, sizeof(recv_data));
	msgrcv(msgid, &recv_data, sizeof(recv_data), 3, 0);
	printf("%s\n",recv_data.msg_data);


	//多功能函数
	//int msqid : 消息队列的ID
	//int cmd   : 可选参数  IPC_STAT 获取消息队列属性 
				//  IPC_SET 设置消息队列属性  IPC_RMID 删除消息队列
	//struct msqid_ds *buf : 如果参数2为 IPC_STAT IPC_SET 用户自己需要定义一个结构体变量
				// 去设置 或 获取消息队列当中的属性 如果为 IPC_RMID 则直接填写为NULL;
	//返回值 成功返回0 失败返回-1

	//获取消息队列的属性
	// struct msqid_ds buf;
	// msgctl(msgid, IPC_STAT, &buf);
	// printf("消息队列中有 %ld 条消息\n",buf.msg_qnum);
	// printf("最后一个发送消息的PID号 %d\n",buf.msg_lspid);

	//设置消息队列的属性(并不是所有的属性都能进行设置)
	// struct msqid_ds buf;
	// buf.msg_perm.mode = 0655;
	// msgctl(msgid, IPC_SET, &buf);


	//删除消息队列
	//msgctl(msgid, IPC_RMID, NULL);

	return 0;
}


//1.使用消息队列实现双向通信，但是需要保证有序的退出.并删除消息队列

//2.使用信号配合消息队列实现双线通信,在收到某个特定信号时,
//  需要在信号处理函数当中.退出程序并删除消息队列

//3.模拟客户端与服务器的数据收发，有3个程序，2个客户端与1个服务器.
//  客户端1 给 客户端2发送数据，由服务器中转.(提升要求：可以连接多个客户端).