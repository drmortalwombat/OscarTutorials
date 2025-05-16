#include <c64/vic.h>
#include <c64/rasterirq.h>
#include <c64/memmap.h>
#include <c64/sprites.h>
#include <string.h>
#include <oscar.h>
#include <math.h>

// Sprite image resource
const char SpriteImage[] = {
	#embed spd_sprites  lzo "../Resources/numballs.spd"
};

// Screen memory
char * const Screen = (char *)0xc000;

char * const Font = (char *)0xd000;

char * const Sprite = (char *)0xe000;

__interrupt void sprmuxirq_call(void)
{
	vic.color_border++;
	vspr_update();
	rirq_sort(true);
	vic.color_border--;
}

static RIRQCode	sprmuxrirq;

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

	rirq_build(&sprmuxrirq, 1);
	rirq_call(&sprmuxrirq, 0, sprmuxirq_call);

	rirq_set(VSPRITES_MAX - 8, 250, &sprmuxrirq);

	// initialize sprites
	for(int i=0; i<24; i++)
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

	int spx[24], spy[24];

	unsigned j = 0;
	for(;;)
	{
		// take a long time to calculate sprite position
		for(char i=0; i<24; i++)
		{
			spx[i] = 172 + int(100 * cos((j + 4 * i) * (PI / 48)));
			spy[i] = 140 + int(100 * sin((j + 4 * i) * (PI / 48)));
		}
		j++;

		// now wait for a safe spot to update the sprites
		vic.color_border = VCOL_YELLOW;
		vic_waitRange(50, 150);
		vic.color_border = VCOL_BLUE;

		// update the sprites
		for(char i=0; i<24; i++)
			vspr_move(i, spx[i], spy[i]);

		// sort virtual sprites by y position
		vspr_sort();

		vic.color_border = VCOL_BLACK;
		vic_waitBottom();
	}

	return 0;
}
