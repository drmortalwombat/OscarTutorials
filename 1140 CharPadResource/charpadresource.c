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
const char MilitaryFont[] = {
	#embed ctm_chars lzo "../Resources/militaryfont.ctm"
};

int main(void)
{
	// Install memory trampoline to ensure system
	// keeps running when playing with the PLA
	mmap_trampoline();

	// Swap in all RAM
	mmap_set(MMAP_RAM);

	oscar_expand_lzo(Charset, MilitaryFont);

	// Swap ROM back in
	mmap_set(MMAP_ROM);

	// Fill screen with all chars
	for(unsigned i=0; i<1000; i++)	
		Screen[i] = i & 63;
	
	// Color all cells yellow
	memset(Color, VCOL_YELLOW, 1000);

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
