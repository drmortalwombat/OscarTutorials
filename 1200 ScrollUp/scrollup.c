#include <c64/vic.h>
#include <stdlib.h>
#include <string.h>

char * const Screen = (char *)0x0400;

void scroll_up(void)
{
	// Naiive implementation using library copy, takes longer than
	// a single frame
	memmove(Screen, Screen + 40, 960);
}


void fill_line(void)
{
	// Fill bottom line with new data
	for(char i=0; i<40; i++)
		Screen[40 * 24 + i] = (rand() & 1) + 77;
}

int main(void)
{
	for(;;)
	{
		// Wait for bottom row with border color red
		vic.color_border = VCOL_RED;
		vic_waitBottom();

		// Border color black during copy
		vic.color_border = VCOL_BLACK;
		scroll_up();

		// Border color white during fill of new row
		vic.color_border = VCOL_WHITE;
		fill_line();
	}

	return 0;
}
