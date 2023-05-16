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

// Cordic atan table
static const char arortab[6] = {32, 19, 10, 5, 3, 1};

// Calculate inverse tangens using cordic with 8 bit precision
char matanu(char dx, signed char dy)
{
	// Accumulated angle
	signed char	sum = 0;

	// Sacrifice one bit of precision to avoid overflow
	if (dx & 0x80)
	{
		dx >>= 1;
		dy >>= 1;
	}

	// First step of cordic
	char sx = dx;
	signed char sy = dy;

	if (dy > 0)
	{
		dx += sy;
		dy -= sx;
		sum = arortab[0];
	}
	else if (dy < 0)
	{
		dx -= sy;
		dy += sx;
		sum = -arortab[0];
	}

	// Remaining iterations, reduce to trade off precision for speed
	for(char i=1; i<6; i++)
	{
		char sx = dx >> i;
		signed char sy = dy >> i;

		if (dy > 0)
		{
			dx += sy;
			dy -= sx;
			sum += arortab[i];
		}
		else if (dy < 0)
		{
			dx -= sy;
			dy += sx;
			sum -= arortab[i];
		}
	}

	return sum;
}

// Inverse tangens four all four quadrants
char matan(int dx, signed char dy)
{
	if (dx < 0)
		return 128 - matanu(-dx, dy);
	else
		return matanu( dx, dy);
}

int main(void)
{
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
