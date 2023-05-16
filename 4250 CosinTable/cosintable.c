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

static signed char sintab[256];

int main(void)
{
	// Init sine table
	for(int i=0; i<256; i++)
		sintab[i] = 64 * sin(i * PI / 128.0);

	// Init sprites
	memcpy(Sprite, SpriteImage, 64);

	spr_init(Screen);

	spr_set(0, true, CenterX, CenterY, (unsigned)Sprite / 64, VCOL_WHITE, false, false, false);
	spr_set(1, true, CenterX, CenterY, (unsigned)Sprite / 64, VCOL_BLACK, false, false, false);

	char w = 0;
	for(;;)
	{
		// Calculate position on circle using lookup table
		vic.color_border++;
		int sx = sintab[(w + 64) & 0xff];
		int sy = sintab[w];
		vic.color_border--;

		// Move sprite
		spr_move(0, CenterX + sx, CenterY + sy);

		// Advance
		w++;

		vic_waitFrame();
	}

	return 0;

}
