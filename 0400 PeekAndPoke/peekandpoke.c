#include <c64/types.h>

// Inline function definition for a poke
inline void poke(unsigned addr, byte value)
{
	// Cast address int a volatile pointer, and assign the
	// referenced memory cell with the value
	*(volatile char *)addr = value;
}

// Inline function definition for a peek
inline byte peek(unsigned addr)
{
	// Cast address into a volatile pointer, and read
	// from the memory location
	return *(volatile char *)addr;
}

int main(void)
{
	for(;;)
	{
		// Change border and background color
		poke(0xd020, 0);
		poke(0xd021, 0);

		// Wait for raster to leave screen
		while (peek(0xd011) < 0x80)
			;

		// Change border and background color
		poke(0xd020, 1);
		poke(0xd021, 1);

		// Wait for raster to reenter screen
		while (peek(0xd011) >= 0x80)
			;
	}

	return 0;
}
