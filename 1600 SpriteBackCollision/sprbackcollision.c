#include <c64/sprites.h>
#include <c64/vic.h>
#include <c64/joystick.h>
#include <string.h>
#include <stdlib.h>

static char * const Screen	=	(char *)0x0400;
static char * const Color	=	(char *)0xd800; 
static char * const Sprites =	(char *)0x0340;

// Sprite data for a an 8x8 square sprite
static const char spimage[63] = {
	0b11111111, 0b00000000, 0b00000000,
	0b10000001, 0b00000000, 0b00000000,
	0b10000001, 0b00000000, 0b00000000,
	0b10000001, 0b00000000, 0b00000000,
	0b10000001, 0b00000000, 0b00000000,
	0b10000001, 0b00000000, 0b00000000,
	0b10000001, 0b00000000, 0b00000000,
	0b11111111, 0b00000000, 0b00000000,
	0b00000000, 0b00000000, 0b00000000,
	0b00000000, 0b00000000, 0b00000000,
	0b00000000, 0b00000000, 0b00000000,
	0b00000000, 0b00000000, 0b00000000,
	0b00000000, 0b00000000, 0b00000000,
	0b00000000, 0b00000000, 0b00000000,
	0b00000000, 0b00000000, 0b00000000,
};

void display_init(void)
{
	memcpy(Sprites, spimage, 64);
	spr_init(Screen);
	memset(Screen, 32, 1000);
	memset(Color, VCOL_LT_BLUE, 1000);	
}

void char_put(char x, char y, char ch, char color)
{
	Screen[40 * y + x] = ch;
	Color[40 * y + x] = color;
}

char char_get(char x, char y)
{
	return Screen[40 * y + x];
}

// Get character at given screen position in pixel
char char_get_pix(int x, int y)
{
	return Screen[40 * (y >> 3) + (x >> 3)];
}

int main(void)
{
	display_init();

	int spx = 160, spy = 100;

	spr_set(0, true, 0, 0, (unsigned)Sprites / 64, VCOL_WHITE, false, false, false);

	for(char i=0; i<100; i++)
	{
		char_put(rand() % 40, rand() % 25, 160, VCOL_LT_BLUE);
	}

	for(;;)
	{
		joy_poll(0);
		if (joyx[0] < 0 && spx > 0)
			spx--;
		else if (joyx[0] > 0 && spx < 312)
			spx++;

		if (joyy[0] < 0 && spy > 0)
			spy--;
		else if (joyy[0] > 0 && spy < 192)
			spy++;

		spr_move(0, spx + 24, spy + 50);

		// Get the character at the four corner positions of
		// our square sprite		
		char	tl = char_get_pix(spx + 0, spy + 0);
		char	tr = char_get_pix(spx + 7, spy + 0);
		char	bl = char_get_pix(spx + 0, spy + 7);
		char	br = char_get_pix(spx + 7, spy + 7);

		// Color the sprite red, if any of the four corners is on
		// top of a filled char
		spr_color(0, (tl | tr | bl | br) >= 128 ? VCOL_RED : VCOL_WHITE);

		vic_waitFrame();
	}

	return 0;
}
