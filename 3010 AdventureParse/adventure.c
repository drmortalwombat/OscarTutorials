#include <stdio.h>
#include <string.h>
#include "adventure.h"

// Up to four tokens with up to 20 chars	
char	tokens[4][21];
char	ntokens;

void read_tokens(void)
{
	ntokens = 0;
	while (ntokens == 0)
	{
		// Show a prompt
		printf("> ");

		// No tokens, no char
		int  i = 0;

		// Loop until end of line
		while ((char ch = getchar()) != '\n')
		{
			// Still tokenspace remaining
			if (ntokens < 4)
			{
				// White space, so end of word
				if (ch == ' ')
				{
					// Ignore space at start of word
					if (i != 0)
					{
						// Close string
						tokens[ntokens][i] = 0;
						// Next token
						ntokens++;
						i = 0;
					}
				}
				else if (i < 20)
				{
					// Still space in current token, so append it
					tokens[ntokens][i++] = ch;
				}
			}
		}
		// Check if pending token
		if (i > 0)
		{
			// Close string and increment number of tokens
			tokens[ntokens][i] = 0;
			ntokens++;
		}
		printf("\n");
	}
}
