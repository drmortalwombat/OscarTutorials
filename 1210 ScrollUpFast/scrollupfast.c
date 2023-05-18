#include <c64/vic.h>
#include <stdlib.h>

char * const Screen = (char *)0x0400;

// Copy screen up racing the beam.
void scroll_up(void)
{
	// Copy is unrolled vertically to allow for eight bit register
	// indexing of the horizontal position.  So first column is scrolled
	// first, then second and so on.  This conflicts with the raster
	// order from top to bottom, so right side would overlap with screen
	// redraw.  Therefore we split the scroll into an upper and a lower half.

	for(char i=0; i<40; i++)
	{
		// Unroll vertical portion
		#pragma unroll(full)
		for(char j=0; j<12; j++)
			Screen[40 * j + i] = Screen[40 * (j + 1) + i];
	}

	// Second range of rows, will be scrolled while first half is displayed
	for(char i=0; i<40; i++)
	{
		#pragma unroll(full)
		for(char j=12; j<24; j++)
			Screen[40 * j + i] = Screen[40 * (j + 1) + i];
	}
}

void fill_line(void)
{
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
