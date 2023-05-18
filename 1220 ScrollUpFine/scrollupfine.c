#include <c64/vic.h>
#include <stdlib.h>

char * const Screen = (char *)0x0400;

// scroll up by one char line
void scroll_up(void)
{
	for(char i=0; i<40; i++)
	{
		#pragma unroll(full)
		for(char j=0; j<12; j++)
			Screen[40 * j + i] = Screen[40 * (j + 1) + i];
	}
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
			vic.ctrl1 = VIC_CTRL1_DEN | (i ^ 7);

			// Scroll by one char up
			scroll_up();
			fill_line();
		}
		else
		{
			// Update scroll register, hide bottom row, using the xor with
			// seven to revert i
			vic.ctrl1 = VIC_CTRL1_DEN | (i ^ 7);

			// Wait for raster to leave bottom region of screen
			vic_waitTop();
		}
	}

	return 0;
}
