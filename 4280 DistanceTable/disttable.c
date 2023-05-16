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

static char dtab[16][16];

int mdist(int dx, int dy)
{
	// Remove sign
	if (dx < 0)
		dx = -dx;
	if (dy < 0)
		dy = -dy;

	unsigned ux = dx, uy = dy;
	char	s = 0;

	// Shift to right until it fits into 
	// lookup table, first shift uses byte optimization
	if ((ux | uy) & 0xf000)
	{
		ux >>= 8;
		uy >>= 8;
		s += 8;
	}
	// Shift out all remaining bits from high byte in one step
	if ((ux | uy) & 0xff00)
	{
		ux >>= 4;
		uy >>= 4;		
		s += 4;
	}

	// Switch to bytes for the rest
	char bx = ux, by = uy;
	while ((bx | by) & 0xf0)
	{
		bx >>= 1;
		by >>= 1;
		s++;
	}

	// Lookup distance function in table, and shift back into
	// original range
	return dtab[by][bx] << s;
}

int main(void)
{
	// Init lookup table
	for(char y=0; y<16; y++)
		for(char x=0; x<16; x++)
			dtab[y][x] = usqrt(x * x + y * y);

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

		// Calculate distance from center using lookup table
		vic.color_border++;
		int d = mdist(sx, sy);
		vic.color_border--;

		// Print result
		printf("%4d, %4d, %4d\n", sx, sy, d);

		vic_waitFrame();
	}

	return 0;
}
