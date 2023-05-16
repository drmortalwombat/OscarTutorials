#include <math.h>
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
__striped static const int arortab[8] = {8192, 4836, 2555, 1297, 651, 326, 163, 81};

// Calculate sine and cosine using cordic
void cosincc(int w, signed char * si, signed char * co)
{
	// Scaled start vector
	int dx = 9945;
	int dy = 0;

	// Move into correct quadrant
	if (w > 16384 || w < -16384)
	{
		w ^= 0x8000;
		dx = -dx;
	}

	// Eight iterations, reduce to sacrifice precision for speed
	for(char i=0; i<8; i++)
	{
		// Rotation vector
		int sx = dx >> i;
		int sy = dy >> i;

		// Check foward or backward rotation
		if (w > 0)
		{
			// Forward rotation
			dx += sy;
			dy -= sx;
			w -= arortab[i];
		}
		else
		{
			// Backward rotation
			dx -= sy;
			dy += sx;
			w += arortab[i];
		}
	}

	// Return MSBs of coordinates as cos and sin
	*si = dy >> 8;
	*co = dx >> 8;
}

int main(void)
{
	// Init sprites
	memcpy(Sprite, SpriteImage, 64);

	spr_init(Screen);

	spr_set(0, true, CenterX, CenterY, (unsigned)Sprite / 64, VCOL_WHITE, false, false, false);
	spr_set(1, true, CenterX, CenterY, (unsigned)Sprite / 64, VCOL_BLACK, false, false, false);

	// Eight bit angle
	char w = 0;
	for(;;)
	{
		signed char sx, sy;

		// Calculate position on circle using cordic
		vic.color_border++;
		cosincc(w << 8, &sx, &sy);
		vic.color_border--;

		// Move sprite
		spr_move(0, CenterX + sx, CenterY + sy);

		// Advance
		w++;

		vic_waitFrame();
	}

	return 0;

}
