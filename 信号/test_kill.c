#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>

int main(int argc, char const *argv[])
{
	kill(atoi(argv[1]), atoi(argv[2]));
	return 0;
}