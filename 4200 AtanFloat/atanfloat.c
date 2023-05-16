#include <math.h>
#include <stdio.h>
#include <string.h>
#include <c64/joystick.h>
#include <c64/sprites.h>

// Image for cross hair sprite
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

// Center of screen in cross hair sprite coordinates
static const int CenterX = 160 + 24 - 7;
static const int CenterY = 100 + 50 - 7;

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
		float a = atan2(sy, sx);
		vic.color_border--;

		// Print result
		printf("%4d, %4d, %5.3f\n", sx, sy, a);

		// Wait for next frame
		vic_waitFrame();
	}

	return 0;
}
