#include <conio.h>
#include <c64/vic.h>

byte * const Screen = (byte*)0x0400;
byte * const Color = (byte*)0xd800;

int main(void)
{
	char y = 0;
	char x = 0;

	while(true)
	{
		// Fill current cursor position
		Screen[40 * y + x] = 0xa0;

		// Wait for key
		char c = getch();

		// Clear current cursor position
		Screen[40 * y + x] = 0x20;

		// Adapt cursor position according to key
		switch (c)
		{
		case 17:		// Down
			if (y < 24) y++;
			break;
		case 17 + 128:	// Up
			if (y > 0) y--;
			break;
		case 29:		// Right
			if (x < 39) x++;
			break;
		case 29 + 128:	// Left
			if (x > 0) x--;
			break;
		}
	}

	return 0;
}
