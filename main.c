#include <stdio.h>
#include <call.h>
#include"pagetable.h"
//#include <stdlib.h>
#define _CRT_SECURE_NO_WARNINGS
#define Fail(tips) \
        printf(tips); \
        printf("\n");

#define Success(tips) \
        printf(tips); \
        printf("\n");
struct ProcessInfo
{
	int run;
	int length;
	struct Process* p;
};
#define cde 1
int main()
{
	char a[100][100] = { 0 };
	a[1][0] = NULL;
	while (cde)
	{
		if (1)
		{
			break;
		}
	}
	if (a[1][1] == NULL)
	{
		printf("aslkfdja\n");
	}
	if (a[1][0] == NULL)
		printf("haha\n");
	int z=5;
	int y=6;
	z = z + y;
	system("pause");
	return 0;
}