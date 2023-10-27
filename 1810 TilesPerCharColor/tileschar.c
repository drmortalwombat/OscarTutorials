#include <string.h>
#include <stdlib.h>
#include <c64/vic.h>
#include <c64/memmap.h>
#include <oscar.h>
#include <conio.h>

// Custom screen address
char * const Screen = (char *)0xc000;

// Custom charset address
char * const Charset = (char *)0xd000;

// Color mem address
char * const Color = (char *)0xd800;

// The charpad resource in lz compression, without the need
// for binary export
const char NumberFont[] = {
	#embed ctm_chars lzo "../Resources/percharcolor.ctm"
};

// The tile map, a 16x16 grid of 8 bit tile indices
const char NumberMap[] = {
	#embed ctm_map8 "../Resources/percharcolor.ctm"
};

// The tiles, an array of 4x4 char indices
const char NumberTiles[] = {
	#embed ctm_tiles8 "../Resources/percharcolor.ctm"
};

const char NumberAttr[] = {
	#embed ctm_attr1 "../Resources/percharcolor.ctm"
};

#define TILE_MAP_WIDTH		20
#define TILE_MAP_HEIGHT		20
#define TILE_WIDTH 			4
#define TILE_HEIGHT 		4
#define TILE_SCREEN_WIDTH	(40 / TILE_WIDTH)
#define TILE_SCREEN_HEIGHT	(25 / TILE_HEIGHT)

// Display screen at tile x, y
void show(char mx, char my)
{
	const char * mp = NumberMap + TILE_MAP_WIDTH * my + mx;
	char * sp = Screen;
	char * cp = Color;

	for(char ty=0; ty<TILE_SCREEN_HEIGHT; ty++)
	{
		for(char tx=0; tx<TILE_SCREEN_WIDTH; tx++)
		{
			char ti = mp[tx];
			const char * tp = NumberTiles + 16 * ti;

			// Put the tile in one go
			#pragma unroll(full)
			for(char y=0; y<TILE_HEIGHT; y++)
			{
				#pragma unroll(full)
				for(char x=0; x<TILE_WIDTH; x++)
				{
					char c = tp[TILE_WIDTH * y + x];
					sp[40 * y + x] = c;
					cp[40 * y + x] = NumberAttr[c];
				}
			}

			// Next tile
			sp += TILE_WIDTH;
			cp += TILE_WIDTH;
		}

		// Only three lines to advance, we covered one already with the
		// horizontal advance
		sp += 120;
		cp += 120;		

		// Next row of tiles
		mp += TILE_MAP_WIDTH;
	}
}


int main(void)
{
	// Install memory trampoline to ensure system
	// keeps running when playing with the PLA
	mmap_trampoline();

	// Swap in all RAM
	mmap_set(MMAP_RAM);

	oscar_expand_lzo(Charset, NumberFont);

	// Swap ROM back in
	mmap_set(MMAP_ROM);

	// Clear screen, prepare color area for multicolor
	memset(Color, VCOL_BLACK + 8, 1000);
	memset(Screen, 0, 1000);

	// Background and border black
	vic.color_border = VCOL_MED_GREY;
	vic.color_back = VCOL_MED_GREY;
	vic.color_back1 = VCOL_BLACK;
	vic.color_back2 = VCOL_WHITE;

	// Set VIC to show custom screen with custom charset and multicolor
	vic_setmode(VICM_TEXT_MC, Screen, Charset);

	// Position in pixel
	char mx = 0, my = 0;

	show(mx, my);

	for(;;)
	{
		// Check if user pressed key
		char ch = getchx();

		// Set direction based on input
		switch (ch)
		{
		case 17:		// Down
			if (my < TILE_MAP_HEIGHT - TILE_SCREEN_HEIGHT) my++;
			break;
		case 17 + 128:	// Up
			if (my > 0) my--;
			break;
		case 29:		// Right
			if (mx < TILE_MAP_WIDTH - TILE_SCREEN_WIDTH) mx++;
			break;
		case 29 + 128:	// Left
			if (mx > 0) mx--;
			break;
		}

		// Wait for raster to reach bottom of screen
		vic_waitBottom();

		show(mx, my);
	}

	return 0;
}
