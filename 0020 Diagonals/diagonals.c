#include <conio.h>
#include <stdio.h>

int main(void)
{
	while(true)
	{
		putchar(205 + (rand() & 1));
	}
	return 0;
}
