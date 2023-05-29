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
__striped static int arortab[8] = {8192, 4836, 2555, 1297, 651, 325, 162, 81};

// Calculate inverse tangens using cordic
int matan(int dx, int dy)
{
	// Accumulated angle
	int sum = 0;

	// Fix second and third quadrant
	if (dx < 0)
	{
		dx = -dx;
		dy = -dy;
		sum = -32768;		
	}

	// Loop for seven bits precision, use shorter loop to
	// trade precision for speed
	for(char i=0; i<7; i++)
	{
		// Rotation vector
		int sx = dx >> i;
		int sy = dy >> i;

		// Check for forward/backward rotation
		if (dy > 0)
		{
			// Forward rotation
			dx += sy;
			dy -= sx;
			sum += arortab[i];
		}
		else if (dy < 0)
		{
			// Backward rotation
			dx -= sy;
			dy += sx;
			sum -= arortab[i];
		}
	}

	// Get rid of LSB
	return (sum >> 8) & 0xff;
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
