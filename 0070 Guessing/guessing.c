#include <conio.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	iocharmap(IOCHM_PETSCII_2);

	char check;

	do 	{
		int		dice = rand() % 6 + 1;
		int 	guess;

		// Read input until we get a valid guess
		do 	{

			printf("\nInput your guess from 1 to 6:");

			// Response is invalid if it is not a number or a number
			// less than 1 or greater than 6
		} while (scanf("%d", &guess) != 1 || guess < 1 || guess > 6);

		if (guess == dice)
			printf("\nYou guessed right!\n");
		else
			printf("\nSorry, wrong number, the dice rolled %d\n", dice);

		// Read input until we get a valid response
		do {

		 	printf("\n\nTry again (y/n):", check);

		 	// Response is invalid if it is not a char
		 	// or neither a "y" or an "n"
		} while (scanf("%c", &check) != 1 || (check != 'y' && check != 'n'));
		
	} while (check == 'y');

	return 0;
}
