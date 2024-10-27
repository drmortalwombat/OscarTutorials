#include <c64/rasterirq.h>
#include <c64/vic.h>
#include <time.h>
#include <conio.h>
#include <stdlib.h>
#include <stdio.h>

RIRQCode	rirq_on, rirq_off;

// Enable 2MHz in VBlank of C64 mode in C128

int main(void)
{
	// Init the raster IRQ system to use the kernal iterrupt vector
	rirq_init_kernal();

	// Build raster interrupt code for enable
	rirq_build(&rirq_on, 1);
	// First command, set to two MHz
	rirq_write(&rirq_on, 0, (char *)0xd030, 1);
	// Place command at star of border
	rirq_set(0, 250, &rirq_on);

	// Build raster interrupt for disable
	rirq_build(&rirq_off, 1);
	// Set to one MHz again
	rirq_write(&rirq_off, 0, (char *)0xd030, 0);
	// Put the raster interrupt code at raster line 49
	rirq_set(1, 49, &rirq_off);

	// Prepare the raster IRQ order
	rirq_sort();

	// start raster IRQ processing
	rirq_start();

	clock_t start = clock();

	for(int i=0; i<2000; i++)	
		putch(205 + (rand() & 1));

	clock_t end = clock();

	printf(p"Took %d ms\n", (end - start) * 20);
}

