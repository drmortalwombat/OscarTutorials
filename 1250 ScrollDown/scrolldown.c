#include <c64/vic.h>
#include <stdlib.h>

char * const Screen = (char *)0x0400;


// Scroll down by one char, split into two sections
// to prevent the unrolling from colliding with the
// vertical raster direction

// This is slightly more complex than the other scroll
// direction, because scrolling the upper half of the
// screen down, would destroy the first row of the
// lower half.

void scroll_down(void)
{
	char buffer[40];

	// Save top row of bottom half
	for(char i=0; i<40; i++)
		buffer[i] = Screen[40 * 12 + i];

	// Scroll upper half down
	for(char i=0; i<40; i++)
	{
		#pragma unroll(full)
		for(char j=12; j>0; j--)
			Screen[40 * j + i] = Screen[40 * (j - 1) + i];
	}
	
	// Scroll lower half down
	for(char i=0; i<40; i++)
	{
		#pragma unroll(full)
		for(char j=24; j>13; j--)
			Screen[40 * j + i] = Screen[40 * (j - 1) + i];
	}

	// Restore top row of bottom half
	for(char i=0; i<40; i++)
		Screen[40 * 13 + i] = buffer[i];
}

void fill_line(void)
{
	for(char i=0; i<40; i++)
		Screen[i] = (rand() & 1) + 77;
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
			vic.ctrl1 = VIC_CTRL1_DEN | i;

			// Scroll by one char down
			scroll_down();
			fill_line();
		}
		else
		{
			// Update scroll register, hide bottom row			
			vic.ctrl1 = VIC_CTRL1_DEN | i;

			// Wait for raster to leave bottom region of screen
			vic_waitTop();
		}
	}

	return 0;
}
