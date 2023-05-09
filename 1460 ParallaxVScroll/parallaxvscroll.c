#include <c64/vic.h>
#include <c64/rasterirq.h>
#include <string.h>

// Custom screen location
char * const Screen = (char *)0xc800;

// Custom font location
char * const Font = (char *)0xc000;
char * const Color = (char *)0xd800;

// Lines to scroll in.  Chars 1 to 4 will be used for parallax
// There is no way to split the screen into horizontal regions, so this sample uses the character set to
// simulate vertical scrolling of the background characters to simulate different scroll speeds.

static const char Lines[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 3, 4, 4, 4, 4, 4, 4, 3, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 4, 4, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 3, 4, 4, 4, 4, 4, 4, 3, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 4, 4, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 3, 4, 4, 4, 4, 4, 4, 3, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 3, 4, 4, 4, 4, 4, 4, 3, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 4, 4, 4, 4, 4, 4, 3, 2, 2, 1, 1, 1, 0, 0, 0, 0, 4, 4, 4, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 4, 4, 4, 4, 4, 4, 3, 2, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

// Custom char set, first char is foreground, the others are
// parallax source chars
const char CharSet[] = {
	0b11111111,
	0b10111111,
	0b11110111,
	0b11111111,
	0b11011111,
	0b11111111,
	0b11111011,
	0b01111111,

	0b10100100,
	0b00101001,
	0b10010001,
	0b01001010,
	0b10100100,
	0b01010010,
	0b10010001,
	0b10100100,

	0b00100010,
	0b00001000,
	0b01000001,
	0b01000000,
	0b00000100,
	0b01000000,
	0b00010001,
	0b10001000,

	0b00100000,
	0b00001000,
	0b00000001,
	0b01000000,
	0b00000100,
	0b00000000,
	0b00010001,
	0b10000000,

	0b00000000,
	0b00001000,
	0b00000000,
	0b01000000,
	0b00000100,
	0b00000000,
	0b00010001,
	0b00000000
};

// Scroll screen down by one char cell
void scroll_down(void)
{
	char buffer[40];

	for(char i=0; i<40; i++)
		buffer[i] = Screen[40 * 12 + i];

	for(char i=0; i<40; i++)
	{
		#pragma unroll(full)
		for(char j=12; j>0; j--)
			Screen[40 * j + i] = Screen[40 * (j - 1) + i];
	}

	for(char i=0; i<40; i++)
	{
		#pragma unroll(full)
		for(char j=24; j>13; j--)
			Screen[40 * j + i] = Screen[40 * (j - 1) + i];
	}

	for(char i=0; i<40; i++)
		Screen[40 * 13 + i] = buffer[i];
}

// Fill top line of screen
void fill_line(char row)
{
	const char * sp = Lines + (row & 7) * 40;
	for(char i=0; i<40; i++)
		Screen[i] = sp[i];
}

inline void fix_char(char ch, char sy)
{
	for(char i=0; i<8; i++)
		Font[8 * ch + i] = CharSet[8 * ch + ((i + sy) & 7)];
}

int main(void)
{
	// Fill screen with foreground
	memset(Color, VCOL_WHITE, 1000);
	memset(Screen, 0, 1000);

	vic.color_back = VCOL_BLACK;
	vic.color_border = VCOL_BLACK;

	// Custom font and screen
	vic_setmode(VICM_TEXT, Screen, Font);

	// Init font data
	memcpy(Font, CharSet, 40);

	// Scroll offsets for foreground and parallax zones
	char i = 0, j0 = 0, j1 = 0, j2 = 0;
	char row = 0;

	for(;;)
	{
		// Wait for bottom of screen
		vic_waitBottom();

		// Next pixel row
		i++;
		// Slower parallax speeds
		if (i & 1)
			j0++;
		if (i & 3)
			j1++;
		if (i & 7)
			j2++;

		// Correct the four parallax characters to scroll and undo
		// the pixel scroll register offset
		fix_char(1, j0);
		fix_char(2, j1);
		fix_char(3, j2);
		fix_char(4, i);

		// Vertical scroll foreground section
		if (i == 8)
		{	
			i = 0;
			vic.ctrl1 = VIC_CTRL1_DEN | i;

			scroll_down();
			fill_line(row);
			row++;
		}
		else
		{
			vic.ctrl1 = VIC_CTRL1_DEN | i;
			vic_waitTop();
		}
	}

	return 0;
}
