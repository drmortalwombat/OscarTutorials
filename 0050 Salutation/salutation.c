#include <stdio.h>
#include <conio.h>

int main(void)
{
	// Switch to lowercase map
	iocharmap(IOCHM_PETSCII_2);

	// Opening question
	printf("Please enter your name:\n");

	// Buffer to store the response
	char	buffer[41];

	// Read up to 40 alpha characters
	scanf("%40[a-zA-Z ]", buffer);

	// Greet the user
	printf("\n\nHello %s !", buffer);

	return 0;
}
