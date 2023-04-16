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

char * const Screen = (char *)0x0400;
char * const Sprite = (char *)0x0380;


struct RefSprite
{
	int sx, sy, vx, vy;
}	sprites[8];

int main(void)
{
	memcpy(Sprite, SpriteImage, 64);

	spr_init(Screen);

	for(char i=0; i<8; i++)
	{
		sprites[i].sx = rand() % (320 - 24);
		sprites[i].sy = rand() % (200 - 21);
		sprites[i].vx = rand() % 5 - 2;
		sprites[i].vy = rand() % 5 - 2;

		spr_set(i, true, 24 + sprites[i].sx, 50 + sprites[i].sy, (unsigned)Sprite / 64, i + 7, false, false, false);
	}

	while (true)
	{
		for(char i=0; i<8; i++)
		{
			int sx = sprites[i].sx + sprites[i].vx;
			int sy = sprites[i].sy + sprites[i].vy;

			if (sx < 0 || sx > 320 - 24)
				sprites[i].vx = -sprites[i].vx;
			else
				sprites[i].sx = sx;

			if (sy < 0 || sy > 200 - 21)
				sprites[i].vy = -sprites[i].vy;
			else
				sprites[i].sy = sy;

			spr_move(i, 24 + sprites[i].sx, 50 + sprites[i].sy);
		}

		vic_waitFrame();
	}

	return 0;
}
