#include <c64/vic.h>

// Pointer to first byte of screen memory
byte * const Screen = (byte*)0x0400;

int main(void)
{
	// Fill screen memory with all 256 characters
	for(int i=0; i<256; i++)
		Screen[i] = i;

	return 0;
}
