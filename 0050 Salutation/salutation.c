#include <stdio.h>
#include <conio.h>

int main(void)
{
	iocharmap(IOCHM_PETSCII_2);

	printf("Please enter your name:\n");

	char	buffer[41];

	scanf("%40[a-zA-Z ]", buffer);

	printf("\n\nHello %s !", buffer);

	return 0;
}
