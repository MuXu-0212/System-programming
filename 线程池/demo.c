#include <stdio.h>

//旧的任务
int Add(int a,int b)
{
	return a + b;
}

struct data
{
	int a;
	int b;
};

//新的任务
void *New_Add(void *arg)
{
	struct data *p = (struct data *)arg;
	p->a = (p->a) + (p->b);
	return  p;
}

int main(int argc, char const *argv[])
{
	
	struct data tmp;
	tmp.a = 10;
	tmp.b = 20;

	struct data *p = New_Add(&tmp);
	printf("%d\n",p->a);

	return 0;
}

