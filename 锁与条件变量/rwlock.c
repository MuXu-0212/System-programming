#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// 定义了一个读写锁
pthread_rwlock_t rwlock;

// 全局变量，这是典型的共享资源
int global = 0;

void *routine1(void *arg)
{
    //============ WRITE lock ============//
    // 游戏规则：每当你要对临界资源进行写操作的时候，必须加写锁
    // 加了写锁之后，不可加任何其他的锁，写锁就是互斥锁
    //pthread_rwlock_wrlock(&rwlock);
    global += 1;
    printf("I am %s, now global=%d\n", (char *)arg, global);
    sleep(1);

    // 在结束对共享资源的访问之后，一定要解锁，否则会引起死锁
    //pthread_rwlock_unlock(&rwlock);
    //=========== WRITE unlock ===========//

    pthread_exit(NULL);
}

void *routine2(void *arg)
{
    //============ WRITE lock ============//
    // 游戏规则：每当你要对临界资源进行写操作的时候，必须加写锁
    // 加了写锁之后，不可加任何其他的锁，写锁就是互斥锁
    pthread_rwlock_wrlock(&rwlock);
    global  = 100;
    printf("I am %s, now global=%d\n", (char *)arg, global);
    sleep(1);
    pthread_rwlock_unlock(&rwlock);
    //=========== WRITE unlock ===========//

    pthread_exit(NULL);
}

void *routine3(void *arg)
{
    //============ READ lock ============//
    // 游戏规则：每当你要对临界资源进行读操作的时候，必须加读锁
    // 加了读锁之后，可以加其他的读锁，读锁就是共享锁
    pthread_rwlock_rdlock(&rwlock);
    printf("I am %s, now global=%d\n", (char *)arg, global);
    sleep(1);
    pthread_rwlock_unlock(&rwlock);
    //=========== READ unlock ===========//
    pthread_exit(NULL);
}

void *routine4(void *arg)
{
    //============ READ lock ============//
    // 游戏规则：每当你要对临界资源进行读操作的时候，必须加读锁
    // 加了读锁之后，可以加其他的读锁，读锁就是共享锁
    pthread_rwlock_rdlock(&rwlock);
    printf("I am %s, now global=%d\n", (char *)arg, global);
    sleep(1);
    pthread_rwlock_unlock(&rwlock);
    //=========== READ unlock ===========//
    pthread_exit(NULL);
}

int main (int argc, char *argv[])
{
    // 初始化
    pthread_rwlock_init(&rwlock, NULL);

    pthread_t t1, t2, t3, t4;
	pthread_create(&t1, NULL, routine1, "thread 1");
    pthread_create(&t2, NULL, routine2, "thread 2");
	pthread_create(&t3, NULL, routine3, "thread 3");
	pthread_create(&t4, NULL, routine4, "thread 4");

	

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);
    pthread_rwlock_destroy(&rwlock);
    
    return 0;
}