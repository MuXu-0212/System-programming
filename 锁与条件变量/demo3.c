#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <errno.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>


#define PATH "/home/gec"

sem_t *data; 	//数据资源
sem_t *space; 	//空间资源
char *shm_addr;
int shm_id;


void func(int sig)
{
    sem_close(data);
    sem_close(space);

    shmdt(shm_addr);
    shmctl(shm_id,IPC_RMID,NULL);
    sem_unlink("data");
    sem_unlink("space");
}

void func1(void)
{
    sem_close(data);
    sem_close(space);

    shmdt(shm_addr);
    shmctl(shm_id,IPC_RMID,NULL);
    sem_unlink("data");
    sem_unlink("space");
}

int main(int argc, char const *argv[])
{

    signal(SIGINT,func);

    shm_id = shmget(ftok(PATH,3), 1024, IPC_CREAT | IPC_EXCL | 0777);
    if (shm_id == -1)
    {
        if (errno == EEXIST)
        {
            shm_id = shmget(ftok(PATH,3), 1024, 0777);
        }
        else
        {
            perror("申请共享内存失败\n");
            return -1;
        }
    }

    shm_addr = shmat(shm_id,NULL,0);
    if (shm_addr == NULL)
    {
        perror("映射内存失败\n");
        return -2;
    }

    //申请一个信号量,该信号量代表着数据,因为一开始我们是没有数据的,所以初始为0
    data = sem_open("/data", O_CREAT , 0777, 0);
    if(data == SEM_FAILED)
    {
        printf("申请信号量失败.\n");
        return -3;
    }
    
    //申请一个信号量,该信号量代表着数据的使用权,如果我们把数据放到空间中,该空间的资源会-1;
    space = sem_open("/space",O_CREAT , 0777, 1);
    if(space == SEM_FAILED)
    {
        printf("申请信号量失败.\n");
        return -3;
    }

    while(1)
    {
        //申请空间写入数据
        sem_wait(space);
        fgets(shm_addr,1024,stdin);
        sem_post(data);

        if (strncmp(shm_addr,"over",4) == 0)
        {
            break;
        }
    }

    func1();

    return 0;
}