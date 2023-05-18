#include <c64/vic.h>
#include <stdlib.h>

char * const Screen = (char *)0x0400;

// Scroll right by one char, split into two sections
// to prevent the unrolling from colliding with the
// vertical raster direction

void scroll_right(void)
{
	for(char i=39; i>0; i--)
	{
		#pragma unroll(full)
		for(char j=0; j<12; j++)
			Screen[40 * j + i] = Screen[40 * j + i - 1];
	}
	for(char i=39; i>0; i--)
	{
		#pragma unroll(full)
		for(char j=12; j<25; j++)
			Screen[40 * j + i] = Screen[40 * j + i - 1];
	}
}

// Redraw left most column
void fill_column(void)
{
	for(char i=0; i<25; i++)
		Screen[40 * i] = (rand() & 1) + 77;
}

int main(void)
{
	// Pixel offset from 0..7
	char i = 0;
	for(;;)
	{
		// Wait for raster to reach bottom of screen
		vic_waitBottom();

		// Advance by one pixel
		i++;

		// Check if scrolled by full char in total
		if (i == 8)
		{	
			// Reset counter and scroll register
			i = 0;
			vic.ctrl2 = i;

			// Scroll by one char right
			scroll_right();
			fill_column();
		}
		else
		{
			// Update scroll register, hide left and right column
			vic.ctrl2 = i;

			// Wait for raster to leave bottom region of screen
			vic_waitTop();
		}
	}

	return 0;
}
