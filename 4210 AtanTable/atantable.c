#include <math.h>
#include <stdio.h>
#include <string.h>
#include <c64/joystick.h>
#include <c64/sprites.h>

static const char SpriteImage[64] = {
	0x01, 0x00, 0x00,
	0x01, 0x00, 0x00,
	0x01, 0x00, 0x00,
	0x01, 0x00, 0x00,
	0x01, 0x00, 0x00,
	0x00, 0x00, 0x00,
	0xfc, 0x7e, 0x00,
	0x00, 0x00, 0x00,
	0x01, 0x00, 0x00,
	0x01, 0x00, 0x00,
	0x01, 0x00, 0x00,
	0x01, 0x00, 0x00,
	0x01, 0x00, 0x00,	
};

char * const Screen = (char *)0x0400;
char * const Sprite = (char *)0x0380;

static const int CenterX = 160 + 24 - 7;
static const int CenterY = 100 + 50 - 7;

// Table of inverse tangens
static char atab[16][16];

// Lookup inverse tangens in first quadrant
unsigned uatan(unsigned dx, unsigned dy)
{
	// Shift to right until it fits into 
	// lookup table, first shift uses byte optimization
	if ((dx | dy) & 0xf000)
	{
		dx >>= 8;
		dy >>= 8;
	}
	// Shift out all remaining bits from high byte in one step
	if ((dx | dy) & 0xff00)
	{
		dx >>= 4;
		dy >>= 4;		
	}

	// Switch to bytes for the rest
	char ux = dx, uy = dy;
	while ((ux | uy) & 0xf0)
	{
		ux >>= 1;
		uy >>= 1;
	}

	// Lookup inverse tangens for the remaining values
	return atab[uy][ux];
}

// Lookup inverse tangens for all quadrants
int matan(int dx, int dy)
{
	if (dx < 0)
	{
		if (dy < 0)
			return 0x80 + uatan(-dx, -dy);
		else
			return 0x80 - uatan(-dx, dy);
	}
	else if (dy < 0)
		return 0x100 - uatan(dx, -dy);
	else
		return uatan(dx, dy);
}

int main(void)
{
	// Init lookup table
	for(char y=0; y<16; y++)
		for(char x=0; x<16; x++)
			atab[y][x] = (char)(atan2(y, x) * 128 / PI + 0.5);

	// Init sprites
	memcpy(Sprite, SpriteImage, 64);

	spr_init(Screen);

	spr_set(0, true, CenterX, CenterY, (unsigned)Sprite / 64, VCOL_WHITE, false, false, false);
	spr_set(1, true, CenterX, CenterY, (unsigned)Sprite / 64, VCOL_BLACK, false, false, false);

	// Relative sprite position
	int sx = 0, sy = 0;
	for(;;)
	{
		// Move sprite with joystick
		joy_poll(0);
		sx += joyx[0];
		sy += joyy[0];
		spr_move(0, CenterX + sx, CenterY + sy);

		// Calculate inverse tangens of sprite position, use border color
		// to show raster time used
		vic.color_border++;
		int ai = matan(sx, sy);
		vic.color_border--;

		// Reference value
		int af = (int)(atan2(sy, sx) * 128 / PI) & 0xff;

		// Print result
		printf("%4d, %4d, %3d : %3d\n", sx, sy, ai, af);

		vic_waitFrame();
	}

	return 0;
}
