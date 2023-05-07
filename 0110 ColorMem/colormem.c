#include <c64/vic.h>

// Pointer to first byte of screen memory
byte * const Screen = (byte*)0x0400;

// Pointer of first byte of color memory
byte * const Color = (byte*)0xd800;

int main(void)
{
	// Iterate over all 25 lines
	for(char y=0; y<25; y++)
	{
		// Iterate over all columns
		for(char x=0; x<40; x++)
		{
			// Put a negative space (full character cell)
			Screen[40 * y + x] = 160;

			// Vertical colour bars
			Color[40 * y + x] = 2 * x / 5;
		}
	}

	return 0;
}
