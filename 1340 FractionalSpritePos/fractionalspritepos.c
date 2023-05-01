#include <c64/vic.h>
#include <c64/sprites.h>
#include <string.h>
#include <stdlib.h>

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

// Screen memory
char * const Screen = (char *)0x0400;

// Unused system memory for sprite image
char * const Sprite = (char *)0x0380;


struct RefSprite
{
	int sx, sy, vx, vy;
}	sprites[8];

#define FBITS	4
#define	FMUL	(1 << FBITS)

int main(void)
{
	// Copy sprite image into VIC available area
	memcpy(Sprite, SpriteImage, 64);

	// Initialize sprite data structures
	spr_init(Screen);

	for(char i=0; i<8; i++)
	{
		// Random position
		sprites[i].sx = (rand() % (320 - 21)) << FBITS;
		sprites[i].sy = (rand() % (200 - 21)) << FBITS;

		// Random speed
		sprites[i].vx = rand() % 81 - 40;
		sprites[i].vy = rand() % 81 - 40;

		// Initialize sprite
		spr_set(i, true, 24 + (sprites[i].sx >> FBITS), 50 + (sprites[i].sy >> FBITS), (unsigned)Sprite / 64, VCOL_BLACK, false, false, false);
	}

	while (true)
	{
		// Read collision flags from VIC status
		char collflags = vic.spr_sprcol;
			
		for(char i=0; i<8; i++)
		{
			// Advance position			
			int sx = sprites[i].sx + sprites[i].vx;
			int sy = sprites[i].sy + sprites[i].vy;

			// Check for reflection at left/right border
			if (sx < 0 || sx > ((320 - 21) << FBITS))
				sprites[i].vx = -sprites[i].vx;
			else
				sprites[i].sx = sx;

			// Check for reflection at top/bottom border
			if (sy < 0 || sy > ((200 - 21) << FBITS))
				sprites[i].vy = -sprites[i].vy;
			else
				sprites[i].sy = sy;

			// Update sprite position
			spr_move(i, 24 + (sprites[i].sx >> FBITS), 50 + (sprites[i].sy >> FBITS));

			// Colorize colliding sprites yellow
			spr_color(i, (collflags & (1 << i)) ? VCOL_YELLOW : VCOL_BLACK);
		}

		// Wait one frame
		vic_waitFrame();
	}

	return 0;
}
