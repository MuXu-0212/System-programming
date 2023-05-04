#include "thread_pool.h"


thread_pool *pool;

void copy_file(char *dest_file,char *src_file)
{
	char buf[1024]={0};
	int fs=open(src_file, O_RDWR);
	
	int fd=open(dest_file, O_CREAT|O_RDWR,0777);
	
	while(1)
	{
		bzero(buf,1024);
		int n=read(fs,buf,1024);
		write(fd,buf,n);
		if(n<100)
		{
			break;
		}
	}
	
	close(fs);
	close(fd);
}


void  copy_dir(char *dest_dir, char *src_dir)
{
	char dest_buffer[256]={0};//用于记录传递过来的目录名
	char src_buffer[256]={0};
	strcpy(dest_buffer,dest_dir);
	strcpy(src_buffer,src_dir);
	
	char dest_buf[256]={0};//记录遍历到的子目录和文件名，绝对路径
	char src_buf[256]={0};
	
	//打开目录
	DIR *pe=opendir(src_buffer);
	if(pe==NULL)
	{
		perror("打开目录失败");
		return;
	}
	
	//遍历目录
	struct dirent *p=NULL;
	
	while(1)
	{
		bzero(dest_buf,256);  //清空
		bzero(src_buf,256);
		p=readdir(pe);//遍历目录
		if(p==NULL)
		{
			break;
		}
		if(!strcmp(p->d_name,".")||!strcmp(p->d_name,".."))
		{
			continue;
		}
	
		struct stat info;
		bzero(&info,sizeof(info));
		
		strcat(dest_buf,dest_buffer);//传递过来的目录拼接上现在遍历的子目录或者文件名
		strcat(dest_buf,"/");
		strcat(dest_buf,p->d_name);
		
		strcat(src_buf,src_buffer);
		strcat(src_buf,"/");
		strcat(src_buf,p->d_name);
		
		stat(src_buf,&info);//获取遍历到的文件属性，以绝对路径获取文件属性
		if(S_ISREG(info.st_mode))//普通文件
		{
			//copy_file(dest_buf,src_buf);
			add_task(pool, copy_file, dest_buf,src_buf);//投放任务
		}
		else if(S_ISDIR(info.st_mode))//目录文件
		{
			umask(0);
			mkdir(dest_buf,0777);
			copy_dir(dest_buf,src_buf);
		}	
	}

	//关闭目录
	closedir(pe);
}



//8、						     ./main  /home/gec/aaa/  /home/gec/bbb/
								//./main ../../aaa/   ../../bbb/
int main(int argc,char **argv)//./main  dir_dest  dir_src
{
	// 1, 堆空间申请一个线程池结构体
	pool = malloc(sizeof(thread_pool));
	//初始化线程池（将申请的线程池的结构体指针传参，创建线程的个数）
	init_pool(pool, 5);
	
	if(access(argv[1],F_OK))//判断文件是否存在
	{
		return -1;
	}
	
	if(access(argv[2],F_OK))//判断文件是否存在
	{
		return -1;
	}
	
	char path_buf[128]={0};
	char src_buf[128]={0};
	char dest_buf[128]={0};
	getcwd(path_buf, 128);//获取当前工作目录的绝对路径
	if(argv[1][0]=='/')//绝对路径
	{
		strncpy(dest_buf,argv[1],128);
	}
	else
	{
		sprintf(dest_buf,"%s/%s",path_buf,argv[1]);//将相对路径拼接成绝对路径
	}
	
	if(argv[2][0]=='/')//绝对路径
	{
		strncpy(src_buf,argv[2],128);
	}
	else
	{
		sprintf(src_buf,"%s/%s",path_buf,argv[2]);//将相对路径拼接成绝对路径
	}
	
	//目录复制
	copy_dir(dest_buf,src_buf);

	// 7, 销毁线程池
	destroy_pool(pool);
	return 0;
}













