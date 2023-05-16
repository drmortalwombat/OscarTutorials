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

int main(void)
{
	// Init sprites
	memcpy(Sprite, SpriteImage, 64);

	spr_init(Screen);

	spr_set(0, true, CenterX, CenterY, (unsigned)Sprite / 64, VCOL_WHITE, false, false, false);
	spr_set(1, true, CenterX, CenterY, (unsigned)Sprite / 64, VCOL_BLACK, false, false, false);

	// Sprite coordinates
	int sx = 0, sy = 0;
	for(;;)
	{
		// Move sprite with joystick
		joy_poll(0);
		sx += joyx[0];
		sy += joyy[0];
		spr_move(0, CenterX + sx, CenterY + sy);

		// Calculate distance from center using integer square root
		vic.color_border++;
		int d = usqrt(sx * sx + sy * sy);
		vic.color_border--;

		// Print result
		printf("%4d, %4d, %4d\n", sx, sy, d);

		vic_waitFrame();
	}

	return 0;
}
