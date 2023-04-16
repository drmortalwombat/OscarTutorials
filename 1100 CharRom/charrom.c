#include <c64/memmap.h>
#include <stdio.h>

void expand8(char c, char * s)
{
	for(char i=0; i<8; i++)
	{
		s[i] = (c & 0x80) ? '#' : '.';
		c <<= 1;
	}
	s[8] = 0;
}

int main(void)
{
	mmap_trampoline();

	const char * romp = (const char *)0xd000;

	for(unsigned i=0; i<4096; i++)
	{
		char	exs[9];
		mmap_set(MMAP_CHAR_ROM);
		char	c = romp[i];
		mmap_set(MMAP_ROM);
		expand8(c, exs);

		printf("%03x : %02x - %s\n", i, c, exs);
	}

	return 0;
}
