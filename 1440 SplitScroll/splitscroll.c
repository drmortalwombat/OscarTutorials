#include <c64/vic.h>
#include <c64/rasterirq.h>
#include <stdlib.h>

char * const Screen = (char *)0x0400;

void scroll_left(void)
{
	for(char i=0; i<39; i++)
	{
		#pragma unroll(full)
		for(char j=0; j<12; j++)
			Screen[40 * j + i] = Screen[40 * j + i + 1];
	}
}

void scroll_right(void)
{
	for(char i=39; i>0; i--)
	{
		#pragma unroll(full)
		for(char j=13; j<25; j++)
			Screen[40 * j + i] = Screen[40 * j + i - 1];
	}
}

void fill_columns(void)
{
	for(char i=0; i<12; i++)
		Screen[40 * i + 39] = (rand() & 1) + 77;
	for(char i=13; i<25; i++)
		Screen[40 * i] = (rand() & 1) + 77;
}

// Raster interrupt command structure for change to scrolled and back

RIRQCode	scroll_top, scroll_mid, scroll_bottom;

int main(void)
{
	// Init the raster IRQ system to use the kernal iterrupt vector
	rirq_init_kernal();

	// Upper scroll area
	rirq_build(&scroll_top, 1);
	// Change control register two with this IRQ
	rirq_write(&scroll_top, 0, &vic.ctrl2, 0);
	// Put it above the top of the screen 
	rirq_set(0, 40, &scroll_top);

	// Center non scroll area
	rirq_build(&scroll_mid, 1);
	// re-enable left and right column and reset horizontal scroll
	rirq_write(&scroll_mid, 0, &vic.ctrl2, VIC_CTRL2_CSEL);
	// Put it onto the end of the top scroll area
	rirq_set(1, 50 + 12 * 8, &scroll_mid);

	// Bottom scroll area
	rirq_build(&scroll_bottom, 1);
	// Change control register two with this IRQ
	rirq_write(&scroll_bottom, 0, &vic.ctrl2, 0);
	// place this at the start of the bottom scroll area
	rirq_set(2, 50 + 13 * 8, &scroll_bottom);

	// sort the raster IRQs
	rirq_sort();

	// start raster IRQ processing
	rirq_start();

	int	x = 0;
	for(;;)
	{
		// wait for raster reaching bottom of screen
		rirq_wait();

		// Update raster IRQ for both scroll regions
		rirq_data(&scroll_top, 0, 7 - (x & 7));
		rirq_data(&scroll_bottom, 0, x & 7);

		// Check if crossing a character boundary
		if ((x & 7) == 0)
		{
			// Copy regions
			scroll_left();
			scroll_right();

			// Add new columns
			fill_columns();
		}

		// Next pixel
		x++;
	}

}