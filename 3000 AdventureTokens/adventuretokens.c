#include <stdio.h>


int main(void)
{
	// Up to four tokens with up to 20 chars	
	char	tokens[4][21];

	for(;;)
	{
		// Show a prompt
		printf("> ");

		// No tokens, no char
		int  n = 0, i = 0;

		// Loop until end of line
		while ((char ch = getchar()) != '\n')
		{
			// Still tokenspace remaining
			if (n < 4)
			{
				// White space, so end of word
				if (ch == ' ')
				{
					// Ignore space at start of word
					if (i != 0)
					{
						// Close string
						tokens[n][i] = 0;
						// Next token
						n++;
						i = 0;
					}
				}
				else if (i < 20)
				{
					// Still space in current token, so append it
					tokens[n][i++] = ch;
				}
			}
		}
		// Check if pending token
		if (i > 0)
		{
			// Close string and increment number of tokens
			tokens[n][i] = 0;
			n++;
		}

		// Print the list of tokens, we received
		printf("\n");
		for(int i=0; i<n; i++)
			printf("YOU SAID <%s>\n", tokens[i]);
	}

	return 0;
}
