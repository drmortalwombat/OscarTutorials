#include <c64/vic.h>

byte * const Screen = (byte*)0x0400;
byte * const Color = (byte*)0xd800;

int main(void)
{
	for(char y=0; y<25; y++)
	{
		for(char x=0; x<40; x++)
		{
			Screen[40 * y + x] = 160;
			Color[40 * y + x] = 2 * x / 5;
		}
	}

	return 0;
}
