#include <string.h>
#include <stdlib.h>
#include <c64/vic.h>
#include <c64/memmap.h>
#include <oscar.h>
#include <conio.h>

// Custom screen address
char * const Screen = (char *)0xc000;

// Custom screen address
char * const Hires = (char *)0xe000;

// Color mem address
char * const Color = (char *)0xd800;

// The charpad resource in lz compression, without the need
// for binary export
const char BallsFont[] = {
	#embed ctm_chars "../Resources/petilebitmapmc.ctm"
};

// The tile map, a 16x16 grid of 8 bit tile indices
const char BallsMap[] = {
	#embed ctm_map8 "../Resources/petilebitmapmc.ctm"
};

// The tiles, an array of 4x4 char indices
const char BallsTiles[] = {
	#embed ctm_tiles8 "../Resources/petilebitmapmc.ctm"
};

const char BallsAttr1[] = {
	#embed ctm_attr1 "../Resources/petilebitmapmc.ctm"
};

const char BallsAttr2[] = {
	#embed ctm_attr2 "../Resources/petilebitmapmc.ctm"
};

#pragma align(BallsFont, 256)
#pragma align(BallsMap, 256)

#define TILE_MAP_WIDTH		20
#define TILE_MAP_HEIGHT		20
#define TILE_WIDTH 			4
#define TILE_HEIGHT 		4
#define TILE_SCREEN_WIDTH	(40 / TILE_WIDTH)
#define TILE_SCREEN_HEIGHT	(25 / TILE_HEIGHT)

// Display screen at tile x, y
void show(char mx, char my)
{
	const char * mp = BallsMap + TILE_MAP_WIDTH * my + mx;
	char * sp = Screen;
	char * cp = Color;
	char * hp = Hires;

	for(char ty=0; ty<TILE_SCREEN_HEIGHT; ty++)
	{
		for(char tx=0; tx<TILE_SCREEN_WIDTH; tx++)
		{
			char ti = mp[tx];
			const char * tp = BallsTiles + 16 * ti;
			char c1 = BallsAttr1[ti];
			char c2 = BallsAttr2[ti];

			for(char y=0; y<TILE_HEIGHT; y++)
			{
				for(char x=0; x<TILE_WIDTH; x++)
				{
					const char * dp = BallsFont + 8 * tp[TILE_WIDTH * y + x];
					for(signed char i=7; i>=0; i--)
						hp[i] = dp[i];

					hp += 8;
					cp[x] = c1;
					sp[x] = c2;
				}
				hp += 320 - 8 * TILE_WIDTH;
				cp += 40;
				sp += 40;
			}

			// Next tile
			sp += TILE_WIDTH - 40 * TILE_HEIGHT;
			cp += TILE_WIDTH - 40 * TILE_HEIGHT;
			hp += 8 * TILE_WIDTH - 320 * TILE_HEIGHT;
		}

		// Only three lines to advance, we covered one already with the
		// horizontal advance
		sp += 40 * (TILE_HEIGHT - 1);
		cp += 40 * (TILE_HEIGHT - 1);
		hp += 320 * (TILE_HEIGHT - 1);

		// Next row of tiles
		mp += TILE_MAP_WIDTH;
	}
}


int main(void)
{
	// Install memory trampoline to ensure system
	// keeps running when playing with the PLA
	mmap_trampoline();

	// Clear screen, prepare color area for multicolor
	memset(Color, VCOL_BLACK + 8, 1000);
	memset(Screen, 0, 1000);
	memset(Hires, 0, 8000);

	// Background and border black
	vic.color_border = VCOL_MED_GREY;
	vic.color_back = VCOL_BLACK;

	// Set VIC to show custom screen with custom charset and multicolor
	vic_setmode(VICM_HIRES_MC, Screen, Hires);

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
