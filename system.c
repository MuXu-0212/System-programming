#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/stat.h>

#define PATH_LEN 256

int read_dir(const char* dir)
{
	char path[256];
	DIR* dp;
	struct dirent* sdp;

	dp = opendir(dir);
	if (dp == NULL)
	{
		perror("opendir error");
		return 0;
	}

	while((sdp = readdir(dp) != NULL))
	{
		sprintf(path, "%s/%s", dir, sdp->d_name);
		isfile(path);
	}

	closedir(dp);

	return 1;
}

int isfile(const char* name)
{
	int ret = 0;
	struct stat sbuf;
	ret = stat(name, &sbuf);
	if (ret == -1)
	{
		perror("stat error");
		return 0;
	}

	if (S_ISDIR(sbuf.st_mode))
	{

	}

	printf("%s\t%ld\n", name, sbuf.st_size);
	return 1;
}

//该函数被调用，即已被判定为目录
// void fetchir(const char* dir, void (*fcn)(char*))	
// {

// }



int main(int argc, char const *argv[])
{
	if (argc == 1)
	{
		isfile(".");
	}
	else
	{
		isfile(argv[1]);
	}

	return 0;
}

