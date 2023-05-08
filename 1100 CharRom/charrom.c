#include <c64/memmap.h>
#include <stdio.h>

// Expand a character cell row data into text
void expand8(char c, char * s)
{
	// Loop over all eight pixel
	for(char i=0; i<8; i++)
	{
		// Check MSB and select full/empty char
		s[i] = (c & 0x80) ? '#' : '.';
		// Next bit
		c <<= 1;
	}
	// Close string
	s[8] = 0;
}

int main(void)
{
	// Install memory trampoline to ensure system
	// keeps running when playing with the PLA
	mmap_trampoline();

	// Pointer to the charset in ROM
	const char * romp = (const char *)0xd000;

	// Loop over all bytes in both charsets
	for(unsigned i=0; i<4096; i++)
	{
		// String for one row of char data
		char	exs[9];

		// Swap in char ROM
		mmap_set(MMAP_CHAR_ROM);
		// get the char row
		char	c = romp[i];
		// Swap out char ROM
		mmap_set(MMAP_ROM);

		// Expand the char row into string
		expand8(c, exs);

		// Print line
		printf("%03x : %02x - %s\n", i, c, exs);
	}

	return 0;
}
