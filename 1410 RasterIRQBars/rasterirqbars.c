#include <c64/vic.h>
#include <c64/rasterirq.h>
#include <stdio.h>


RIRQCode	rirq_bar[16];

int main(void)
{
	// Init the raster IRQ system to use the kernal iterrupt vector
	rirq_init_kernal();

	for(char i=0; i<16; i++)
	{
		// Build raster interrupt code for upper split, with two commands
		rirq_build(rirq_bar + i, 2);
		// First command, set the border color to blue
		rirq_write(rirq_bar + i, 0, &vic.color_border, i);
		// Second command, set the background color to light blue
		rirq_write(rirq_bar + i, 1, &vic.color_back, i);

		// Put the raster interrupt code at raster line 100
		rirq_set(i, 111 + 8 * i, rirq_bar + i);
	}

	// Prepare the raster IRQ order
	rirq_sort();

	// start raster IRQ processing
	rirq_start();

	// Do something to show that the raster bars continue without
	// main thread pumping
	for(;;)
	{
		printf("HELLO WORLD ");
	}

	return 0;
}
