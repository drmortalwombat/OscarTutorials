#include <string.h>
#include <stdlib.h>
#include <c64/vic.h>
#include <c64/memmap.h>
#include <oscar.h>

// Custom screen address
char * const Screen = (char *)0xc000;

// Custom charset address
char * const Charset = (char *)0xd000;

// Color mem address
char * const Color = (char *)0xd800;

// The charpad resource in lz compression, without the need
// for binary export
const char LaddersFont[] = {
	#embed ctm_chars lzo "../Resources/ladders.ctm"
};

const char LaddersScreen[] = {
	#embed ctm_map8 lzo "../Resources/ladders.ctm"
};

const char LaddersColor[] = {
	#embed ctm_attr1 "../Resources/ladders.ctm"
};

int main(void)
{
	// Install memory trampoline to ensure system
	// keeps running when playing with the PLA
	mmap_trampoline();

	// Swap in all RAM
	mmap_set(MMAP_RAM);

	oscar_expand_lzo(Charset, LaddersFont);

	// Swap ROM back in
	mmap_set(MMAP_ROM);

	// Expand screen map data into screen memory
	oscar_expand_lzo(Screen, LaddersScreen);

	// Color all cells according to per char color
	// Unrolling the loop to work in 6502 page size chunks
	// to avoid indirect addressing
	#pragma unroll(page)
	for(int i=0; i<1000; i++)
		Color[i] = LaddersColor[Screen[i]];

	// Background and border black
	vic.color_border = VCOL_BLACK;
	vic.color_back = VCOL_BLACK;

	// Set VIC to show custom screen with custom charset
	vic_setmode(VICM_TEXT, Screen, Charset);

	// Loop forever
	while (true)
		;

	return 0;
}
