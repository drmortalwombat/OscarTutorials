#include <conio.h>
#include <stdio.h>

int main(void)
{
	// Loop forever
	while(true)
	{
		// Output random sequences of forward and
		// backward diagonals
		putchar(205 + (rand() & 1));
	}
	return 0;
}
