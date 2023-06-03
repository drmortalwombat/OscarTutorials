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
0, 4, 9, 13, 18, 22, 26, 30, 34, 38, 42, 46, 50, 54, 57, 60, 64, 67, 70, 72, 75, 77, 79, 81, 83, 85, 86, 87, 88, 89, 90, 90, 90, 90, 90, 89, 88, 87, 86, 85, 83, 81, 79, 77, 75, 72, 70, 67, 64, 60, 57, 54, 50, 46, 42, 38, 34, 30, 26, 22, 18, 13, 9, 4, 0, -4, -9, -13, -18, -22, -26, -30, -34, -38, -42, -46, -50, -54, -57, -60, -64, -67, -70, -72, -75, -77, -79, -81, -83, -85, -86, -87, -88, -89, -90, -90, -90, -90, -90, -89, -88, -87, -86, -85, -83, -81, -79, -77, -75, -72, -70, -67, -64, -60, -57, -54, -50, -46, -42, -38, -34, -30, -26, -22, -18, -13, -9, -4
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
	for(int i=0; i<32; i++)
	{
		vspr_set(i, 0, 0, 128 + i, (i & 7) + 8);
	}

	// Enable multi color mode
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
		for(char i=0; i<32; i++)
			vspr_move(i, 172 + sintab[(j + 4 * i) & 127], 140 + sintab[(j + 4 * i + 32) & 127]);

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
