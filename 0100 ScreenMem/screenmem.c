#include <c64/vic.h>

byte * const Screen = (byte*)0x0400;

int main(void)
{
	for(int i=0; i<256; i++)
		Screen[i] = i;

	return 0;
}
