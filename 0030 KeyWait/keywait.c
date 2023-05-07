#include <stdio.h>
#include <conio.h>

int main(void)
{
	// Long counter
	long	li = 0;

	// Loop until a key is pressed
	while (!getchx())
	{
		printf("PRESS KEY TO STOP %ld\n", li);
		li++;
	}
	return 0;
}
