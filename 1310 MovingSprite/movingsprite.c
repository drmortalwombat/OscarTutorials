#include <c64/vic.h>
#include <c64/sprites.h>
#include <string.h>

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


int main(void)
{
	memcpy(Sprite, SpriteImage, 64);

	spr_init(Screen);

	for(char i=0; i<8; i++)
		spr_set(i, true, 30 + 20 * i, 60 + 20 * i, (unsigned)Sprite / 64, i, false, false, false);

	char y = 0;
	while (true)
	{
		for(char i=0; i<8; i++)
			spr_move(i, 30 + 20 * i, 60 + 20 * i + y);
		y++;
		vic_waitFrame();
	}

	return 0;
}
