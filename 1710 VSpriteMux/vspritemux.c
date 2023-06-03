#include <c64/vic.h>
#include <c64/rasterirq.h>
#include <c64/memmap.h>
#include <c64/sprites.h>
#include <string.h>
#include <oscar.h>

// Sprite image resource
const char SpriteImage[] = {
	#embed spd_sprites  lzo "../Resources/numballs.spd"
};

// Screen memory
char * const Screen = (char *)0xc000;

char * const Font = (char *)0xd000;

char * const Sprite = (char *)0xe000;

static const signed char sintab[] = {
0, 3, 7, 10, 14, 17, 20, 24, 27, 30, 33, 36, 39, 42, 44, 47, 49, 52, 54, 56, 58, 60, 62, 63, 65, 66, 67, 68, 69, 69, 70, 70, 70, 70, 70, 69, 69, 68, 67, 66, 65, 63, 62, 60, 58, 56, 54, 52, 49, 47, 44, 42, 39, 36, 33, 30, 27, 24, 20, 17, 14, 10, 7, 3, 0, -3, -7, -10, -14, -17, -20, -24, -27, -30, -33, -36, -39, -42, -44, -47, -49, -52, -54, -56, -58, -60, -62, -63, -65, -66, -67, -68, -69, -69, -70, -70, -70, -70, -70, -69, -69, -68, -67, -66, -65, -63, -62, -60, -58, -56, -54, -52, -49, -47, -44, -42, -39, -36, -33, -30, -27, -24, -20, -17, -14, -10, -7, -3
};

int main(void)
{
	// Ensure bank safety
	mmap_trampoline();

	// Copy char ROM into RAM
	mmap_set(MMAP_CHAR_ROM);
	memcpy(Font, Font, 2048);

	// Expand sprite images
	mmap_set(MMAP_RAM);
	oscar_expand_lzo(Sprite, SpriteImage);

	mmap_set(MMAP_ROM);

	// Move VIC into bank 0xc000 to 0xffff
	vic_setmode(VICM_TEXT, Screen, Font);

	// Clear screen
	memset(Screen, ' ', 1000);

	// Setup colors
	vic.color_border = VCOL_MED_GREY;
	vic.color_back = VCOL_MED_GREY;
	vic.spr_mcolor0 = VCOL_BLACK;
	vic.spr_mcolor1 = VCOL_WHITE;

	// Init the raster IRQ system to use the kernal iterrupt vector
	rirq_init_kernal();

	// initialize sprite multiplexer
	vspr_init(Screen);

	// initialize sprites
	for(int i=0; i<16; i++)
	{
		vspr_set(i, 0, 0, 128 + i, (i & 7) + 8);
	}

	vic.spr_multi = 0xff;

	// initial sort and update
	vspr_sort();
	vspr_update();
	rirq_sort();

	// start raster IRQ processing
	rirq_start();

	unsigned j = 0;
	for(;;)
	{
		// move sprites around
		for(char i=0; i<16; i++)
			vspr_move(i, 172 + sintab[(j + 8 * i) & 127], 140 + sintab[(j + 8 * i + 32) & 127]);

		j++;

		// sort virtual sprites by y position
		vspr_sort();

		// wait for raster IRQ to reach and of frame
		rirq_wait();

		// update sprites back to normal and set up raster IRQ for second set
		vspr_update();

		// sort raster IRQs
		rirq_sort();
	}

	return 0;
}