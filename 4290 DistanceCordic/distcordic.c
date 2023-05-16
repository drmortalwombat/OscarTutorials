#include <stdio.h>
#include <string.h>
#include <c64/joystick.h>
#include <c64/sprites.h>
#include <fixmath.h>

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

// Calculate distance using cordic, rotating initial vector
// to align with x axis and scale back to get length of vector
int mdist(int dx, int dy)
{
	// Fix quadrant with negative x
	char ux = dx < 0 ? -dx : dx;
	signed char uy = dy;

	// Unroll loop, small enough to be worth it
	#pragma unroll(full)
	for(char i=0; i<4; i++)
	{
		// Rotation vector
		char sx = ux >> i;
		signed char sy = uy >> i;

		// Forward or backward rotation
		if (uy > 0)
		{
			ux += sy;
			uy -= sx;
		}
		else
		{
			ux -= sy;
			uy += sx;
		}
	}

	// Adjust vector length using hard coded fix point multiplication
	return (ux + (ux >> 2) - (ux >> 6)) >> 1;
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

		// Calculate distance from center using cordic
		vic.color_border++;
		int d = mdist(sx, sy);
		vic.color_border--;

		// Print result
		printf("%4d, %4d, %4d\n", sx, sy, d);

		vic_waitFrame();
	}

	return 0;
}
