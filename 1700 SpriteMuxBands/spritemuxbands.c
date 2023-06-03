#include <c64/vic.h>
#include <c64/rasterirq.h>
#include <c64/memmap.h>
#include <c64/sprites.h>
#include <string.h>
#include <oscar.h>


const char SpriteImage[] = {
	#embed spd_sprites  lzo "../Resources/numballs.spd"
};

// Screen memory
char * const Screen = (char *)0xc000;

char * const Font = (char *)0xd000;

char * const Sprite = (char *)0xe000;

static const char sintab[] = 
	{10, 12, 14, 16, 17, 18, 19, 20, 20, 20, 19, 18, 17, 16, 14, 12, 10, 8, 6, 4, 3, 2, 1, 0, 0, 0, 1, 2, 3, 4, 6, 8};
	

RIRQCode	*	rirq_bands[4];

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

	// Init sprite system
	spr_init(Screen);

	// Move VIC into bank 0xc000 to 0xffff
	vic_setmode(VICM_TEXT, Screen, Font);

	// Clear screen
	memset(Screen, ' ', 1000);

	// Setup colors
	vic.color_border = VCOL_MED_GREY;
	vic.color_back = VCOL_MED_GREY;
	vic.spr_mcolor0 = VCOL_BLACK;
	vic.spr_mcolor1 = VCOL_WHITE;

	// Init sprites
	for(int i=0; i<8; i++)
		spr_set(i, true, 24 + 36 * i, 50, 128 + i, VCOL_PURPLE, true, false, false);

	// Init the raster IRQ system to use the kernal iterrupt vector
	rirq_init_kernal();

	// We use four bands
	char ypos[4];

	for(char i=0; i<4; i++)
	{
		// Start position of the band in raster lines
		ypos[i] = 50 + 50 * i;

		// Build raster interrupt code for sprite band
		rirq_bands[i] = rirq_alloc(16 + 2);

		// Change background color to indicate band start
		rirq_write(rirq_bands[i], 0, &vic.color_back, VCOL_GREEN + i);
		rirq_write(rirq_bands[i], 1, &vic.color_border, VCOL_GREEN + i);

		// Change vertical position and image for all eight sprites
		for(char j=0; j<8; j++)
		{
			rirq_write(rirq_bands[i], j +  2, &vic.spr_pos[j].y, ypos[i]);
			rirq_write(rirq_bands[i], j + 10, Screen + 0x3f8 + j, 128 + 8 * i + j);
		}

		// Put the raster interrupt code at raster slightly above the sprite position
		rirq_set(i, ypos[i] - 5, rirq_bands[i]);
	}

	// Prepare the raster IRQ order
	rirq_sort();

	// start raster IRQ processing
	rirq_start();

	// Do some animation
	char t = 0;
	for(;;)
	{		
		// Wait one frame
		vic_waitFrame();

		// Modify y position of sprites using a sine table
		for(char i=0; i<4; i++)
			for(char j=0; j<8; j++)
				rirq_data(rirq_bands[i], j + 2, ypos[i] + sintab[(t + 8 * i + 4 * j) & 31]);

		// Next frame
		t++;
	}

	return 0;
}
