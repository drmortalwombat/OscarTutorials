#include <c64/vic.h>
#include <stdlib.h>

char * const Screen = (char *)0x0400;

// Scroll left by one char, split into two sections
// to prevent the unrolling from colliding with the
// vertical raster direction

void scroll_left(void)
{
	for(char i=0; i<39; i++)
	{
		#pragma unroll(full)
		for(char j=0; j<12; j++)
			Screen[40 * j + i] = Screen[40 * j + i + 1];
	}
	for(char i=0; i<39; i++)
	{
		#pragma unroll(full)
		for(char j=12; j<25; j++)
			Screen[40 * j + i] = Screen[40 * j + i + 1];
	}
}

// Redraw right most column
void fill_column(void)
{
	for(char i=0; i<25; i++)
		Screen[40 * i + 39] = (rand() & 1) + 77;
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
			vic.ctrl2 = (i ^ 7);

			// Scroll by one char left
			scroll_left();
			fill_column();
		}
		else
		{
			// Update scroll register, hide left and right column
			vic.ctrl2 = (i ^ 7);

			// Wait for raster to leave bottom region of screen
			vic_waitTop();
		}
	}

	return 0;
}
