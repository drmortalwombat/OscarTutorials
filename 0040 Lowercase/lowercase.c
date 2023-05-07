#include <stdio.h>
#include <conio.h>

int main(void)
{
	// Switch display mode to lower case petscii
	iocharmap(IOCHM_PETSCII_2);

	// Print ascii text translated to petscii
	printf("Hello World\n");

	return 0;
}
