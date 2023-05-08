#include <c64/vic.h>
#include <c64/sprites.h>
#include <string.h>
#include <oscar.h>

// Four sprite images with the animation frames of
// a rotating beach ball, data is stored in a .spd file
// Sprite data is lz compressed by the compiler

const char SpriteImage[] = {
	#embed spd_sprites  lzo "../Resources/beachball.spd"
};

// Screen memory
char * const Screen = (char *)0x0400;

// Sprite memory, unused if our program is not too large
char * const Sprite = (char *)0x3f00;


int main(void)
{
	// Expand sprite image into VIC available area
	oscar_expand_lzo(Sprite, SpriteImage);

	// Initialize sprite data structures
	spr_init(Screen);

	// Set sprites on screen
	for(char i=0; i<8; i++)
		spr_set(i, true, 30 + 20 * i, 60 + 20 * i, (unsigned)Sprite / 64, i + 7, false, false, false);

	char y = 0;
	while (true)
	{
		// For all sprites
		for(char i=0; i<8; i++)
		{
			// Move them into new position
			spr_move(i, 30 + 20 * i, 60 + 20 * i + y);
			
			// Advance animation frame every four video frames, 
			// animation has a cycle of four
			// Each sprite has a different starting frame
			char frame = ((y >> 2) + i) & 3;

			// Set mew animation frame
			spr_image(i, (unsigned)Sprite / 64 + frame);
		}

		// Move sprites down
		y++;

		// Wait for next video frame
		vic_waitFrame();
	}

	return 0;
}
