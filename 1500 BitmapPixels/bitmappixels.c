#include <c64/vic.h>
#include <c64/memmap.h>
#include <math.h>
#include <string.h>

// Place bitmap memory underneath kernal ROM
char * const Hires	= (char *)0xe000;

// Place cell color map underneath IO
char * const Screen	= (char *)0xd000;

// Set a single pixel
void set(int x, int y)
{
	// Ensure that pixel is on screen
	if (x >= 0 && x < 320 && y >= 0 && y < 200)
	{
		// Bitmap mode is similar to char mode, but each char cell
		// is eight bytes and represents 8x8 pixels.

		// So the first 8 horizontal pixels are in the first byte, the next
		// are 8 bytes further
		// The first 8 vertical pixels are in bytes 0 to 7, the next is
		// in byte 320 (40 * 8)
		
		char	*	dp = Hires + 40 * 8 * (y >> 3) + (y & 7) + (x & ~7);

		// Set the bit representing the pixel
		*dp |= 0x80 >> (x & 7);
	}
}

int main(void)
{
	// Install trampoline to keep system running with ROM paged out
	mmap_trampoline();

	// Page in RAM
	mmap_set(MMAP_RAM);

	// Set colors
	memset(Screen, 0x01, 1000);

	// Clear hires bitmap
	memset(Hires, 0x00, 8000);

	// Turn IO space back on
	mmap_set(MMAP_NO_ROM);

	// Set hires mode
	vic_setmode(VICM_HIRES, Screen, Hires);
	vic.color_border = VCOL_WHITE;

	float	w = 0.0;
	for(;;)
	{
		// Some fancy curves
		int x = 160 + cos(w) * 80 + cos(w * 5) * 20 + cos(w * 13) * 10;
		int y = 100 + sin(w * 2) * 80 + sin(w * 7) * 20 + sin(w * 11) * 10;

		// Set the pixel
		set(x, y);

		// Advanve
		w += 0.01;
	}

	return 0;
}
