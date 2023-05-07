#include <c64/vic.h>
#include <c64/rasterirq.h>
#include <math.h>


RIRQCode	rirq_top, rirq_bottom;

char sintab[64];

int main(void)
{
	// Prepare sine table for movement
	for(int i=0; i<64; i++)
		sintab[i] = (int)(120 + 60 * sin(i * PI / 32)) | 1;

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

	// Start movement
	char i = 0;
	for(;;)
	{
		// Wait for the interrupts to complete
		rirq_wait();

		// Wait for bottom of screen before updating
		vic_waitBottom();

		// Change raster lines for the two interrupts
		rirq_move(0, sintab[i]);
		rirq_move(1, sintab[i] + 32);

		// Sort the interrupts
		rirq_sort();

		// Next position
		i = (i + 1) & 63;
	}

	return 0;
}
