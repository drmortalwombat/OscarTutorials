#include <c64/sprites.h>
#include <c64/vic.h>
#include <c64/joystick.h>
#include <string.h>
#include <stdlib.h>

static char * const Screen	=	(char *)0x0400;
static char * const Color	=	(char *)0xd800; 
static char * const Sprites =	(char *)0x0340;

// Sprite data for a a 24x21 square sprite
const char SpriteImage[64] = {
	0b00000000, 0b11111000, 0b00000000,
	0b00000011, 0b11111110, 0b00000000,
	0b00001111, 0b11111111, 0b10000000,
	0b00011111, 0b11111111, 0b11000000,
	0b00111111, 0b11111111, 0b11100000,
	0b00111111, 0b11111111, 0b11100000,
	0b01111111, 0b11111111, 0b11110000,
	0b01111111, 0b11111111, 0b11110000,
	0b11111111, 0b11111111, 0b11111000,
	0b11111111, 0b11111111, 0b11111000,
	0b11111111, 0b11111111, 0b11111000,
	0b11111111, 0b11111111, 0b11111000,
	0b11111111, 0b11111111, 0b11111000,
	0b01111111, 0b11111111, 0b11110000,
	0b01111111, 0b11111111, 0b11110000,
	0b00111111, 0b11111111, 0b11100000,
	0b00111111, 0b11111111, 0b11100000,
	0b00011111, 0b11111111, 0b11000000,
	0b00001111, 0b11111111, 0b10000000,
	0b00000011, 0b11111110, 0b00000000,
	0b00000000, 0b11111000, 0b00000000
};


void display_init(void)
{
	memcpy(Sprites, SpriteImage, 64);
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

bool spr_check_char(int sx, int sy, int x, int y)
{
	if (char_get_pix(x, y) >= 128)
	{
		x &= ~7; y &= ~7;

		signed char dx = x - sx;
		signed char dy = y - sy;

		if (dx < -3) dx += 7;
		if (dy < -3) dy += 7;

		if (dx * dx + dy * dy < 121)
			return true;
	}

	return false;
}

// Check if a horizontal line is blocked
bool spr_is_blocked(int x, int y)
{
	return 
		// top row
		spr_check_char(x + 10, y + 10, x, y) ||
		spr_check_char(x + 10, y + 10, x + 8, y) ||
		spr_check_char(x + 10, y + 10, x + 16, y) ||
		spr_check_char(x + 10, y + 10, x + 20, y) ||

		spr_check_char(x + 10, y + 10, x, y + 8) ||
		spr_check_char(x + 10, y + 10, x + 20, y + 8) ||

		spr_check_char(x + 10, y + 10, x, y + 16) ||
		spr_check_char(x + 10, y + 10, x + 20, y + 16) ||

		spr_check_char(x + 10, y + 10, x, y + 20) ||
		spr_check_char(x + 10, y + 10, x + 8, y + 20) ||
		spr_check_char(x + 10, y + 10, x + 16, y + 20) ||
		spr_check_char(x + 10, y + 10, x + 20, y + 20);
}


int main(void)
{
	display_init();

	int spx = 142, spy = 90;

	spr_set(0, true, spx + 24, spy + 50, (unsigned)Sprites / 64, VCOL_WHITE, false, false, false);

	for(char i=0; i<50; i++)
	{
		char_put(rand() % 40, rand() % 25, 160, VCOL_LT_BLUE);
	}

	for(;;)
	{
		joy_poll(0);

		// Move left or right if not colliding
		if (joyx[0] < 0 && spx > 0 && !spr_is_blocked(spx - 1, spy))
			spx--;
		else if (joyx[0] > 0 && spx < 296 && !spr_is_blocked(spx + 1, spy))
			spx++;

		// Move up or down if not colliding
		if (joyy[0] < 0 && spy > 0 && !spr_is_blocked(spx, spy - 1))
			spy--;
		else if (joyy[0] > 0 && spy < 179 && !spr_is_blocked(spx, spy + 1))
			spy++;

		spr_move(0, spx + 24, spy + 50);
		vic_waitFrame();
	}

	return 0;
}
