#include <stdio.h>
#include <conio.h>

int main(void)
{
	long	li = 0;
	while (!getchx())
	{
		printf("PRESS KEY TO STOP %ld\n", li);
		li++;
	}
	return 0;
}
