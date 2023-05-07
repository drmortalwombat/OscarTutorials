#include <c64/vic.h>
#include <c64/rasterirq.h>
#include <stdio.h>


RIRQCode	rirq_top, rirq_bottom;

int main(void)
{
	// Init the raster IRQ system to use the kernal iterrupt vector
	rirq_init_kernal();

	// Build raster interrupt code for upper split, with two commands
	rirq_build(&rirq_top, 2);
	// First command, set the border color to blue
	rirq_write(&rirq_top, 0, &vic.color_border, VCOL_BLUE);
	// Second command, set the background color to light blue
	rirq_write(&rirq_top, 1, &vic.color_back, VCOL_LT_BLUE);
	// Put the raster interrupt code at raster line 100
	rirq_set(0, 100, &rirq_top);

	// Build raster interrupt code for lower split, with two commands
	rirq_build(&rirq_bottom, 2);
	// First command, set the border color to light blue
	rirq_write(&rirq_bottom, 0, &vic.color_border, VCOL_LT_BLUE);
	// Second command, set the background color to blue
	rirq_write(&rirq_bottom, 1, &vic.color_back, VCOL_BLUE);
	// Put the raster interrupt code at raster line 150
	rirq_set(1, 150, &rirq_bottom);

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
