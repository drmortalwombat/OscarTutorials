#include <c64/vic.h>
#include <c64/sprites.h>
#include <string.h>
#include <stdlib.h>

const char SpriteImage[64] = {
	0b00000000, 0b11111111, 0b00000000, 	
	0b00000111, 0b11111111, 0b11100000, 
	0b00011111, 0b11111111, 0b11111000, 
	0b00111111, 0b11111111, 0b11111100, 
	0b00111111, 0b11111111, 0b11111100, 
	0b01111111, 0b11111111, 0b11111110, 
	0b01111111, 0b11111111, 0b11111110, 
	0b01111111, 0b11111111, 0b11111110, 
	0b11111111, 0b11111111, 0b11111111, 
	0b11111111, 0b11111111, 0b11111111, 
	0b11111111, 0b11111111, 0b11111111, 
	0b01111111, 0b11111111, 0b11111110, 
	0b01111111, 0b11111111, 0b11111110, 
	0b01111111, 0b11111111, 0b11111110, 
	0b00111111, 0b11111111, 0b11111100, 
	0b00111111, 0b11111111, 0b11111100, 
	0b00011111, 0b11111111, 0b11111000, 
	0b00000111, 0b11111111, 0b11100000, 
	0b00000000, 0b11111111, 0b00000000, 	
};

// Screen memory
char * const Screen = (char *)0x0400;

// Unused system memory for sprite image
char * const Sprite = (char *)0x0380;


struct RefSprite
{
	int sx, sy, vx, vy;
}	sprites[8];

int main(void)
{
	// Copy sprite image into VIC available area
	memcpy(Sprite, SpriteImage, 64);

	// Initialize sprite data structures
	spr_init(Screen);

	for(char i=0; i<8; i++)
	{
		// Random position
		sprites[i].sx = rand() % (320 - 24);
		sprites[i].sy = rand() % (200 - 21);

		// Random speed
		sprites[i].vx = rand() % 5 - 2;
		sprites[i].vy = rand() % 5 - 2;

		// Initialize sprite
		spr_set(i, true, 24 + sprites[i].sx, 50 + sprites[i].sy, (unsigned)Sprite / 64, i + 7, false, false, false);
	}

	while (true)
	{
		for(char i=0; i<8; i++)
		{
			// Advance position			
			int sx = sprites[i].sx + sprites[i].vx;
			int sy = sprites[i].sy + sprites[i].vy;

			// Check for reflection at left/right border
			if (sx < 0 || sx > 320 - 24)
				sprites[i].vx = -sprites[i].vx;
			else
				sprites[i].sx = sx;

			// Check for reflection at top/bottom border
			if (sy < 0 || sy > 200 - 21)
				sprites[i].vy = -sprites[i].vy;
			else
				sprites[i].sy = sy;

			// Update sprite position
			spr_move(i, 24 + sprites[i].sx, 50 + sprites[i].sy);
		}

		// Wait one frame
		vic_waitFrame();
	}

	return 0;
}
