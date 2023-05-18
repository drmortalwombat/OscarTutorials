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
const char PipeFont[] = {
	#embed ctm_chars lzo "../Resources/pipes.ctm"
};

// The tile map, a 16x16 grid of 8 bit tile indices
const char PipeMap[] = {
	#embed ctm_map8 "../Resources/pipes.ctm"
};

// The tiles, an array of 4x4 char indices
const char PipTiles[] = {
	#embed ctm_tiles8 "../Resources/pipes.ctm"
};

// Draw a segment of the tile map
void tile_expand(char tx, char ty)
{
	// Start of the tilemap, indexed by the offset coordinate
	const char	*	mp = PipeMap + tx + 16 * ty;

	// Target screen pointer
	char * sp = Screen;

	// Six tiles vertically
	for(char y=0; y<6; y++)
	{
		// Ten tiles horizontaly
		for(char x=0; x<10; x++)
		{
			// Base pointer for the tile to expand
			const char * tp = PipTiles + 16 * mp[x];

			// Loop over all 4x4 tile chars, full unroll of both loops
			// for maximum performance
			#pragma unroll(full)
			for(char iy=0; iy<4; iy++)
			{
				#pragma unroll(full)
				for(char ix=0; ix<4; ix++)
				{
					sp[40 * iy + ix] = tp[4 * iy + ix];
				}
			}

			// Next screen position
			sp += 4;
		}

		// Next line of tiles
		mp += 16;

		// Next screen position, already moved one screen line
		// forward while expanding, so only three more lines to go
		sp += 40 * 3;
	}
}

int main(void)
{
	// Install memory trampoline to ensure system
	// keeps running when playing with the PLA
	mmap_trampoline();

	// Swap in all RAM
	mmap_set(MMAP_RAM);

	oscar_expand_lzo(Charset, PipeFont);

	// Swap ROM back in
	mmap_set(MMAP_ROM);

	// Clear screen, prepare color area for multicolor
	memset(Color, VCOL_BLUE + 8, 1000);
	memset(Screen, 0, 1000);

	// Background and border black
	vic.color_border = VCOL_MED_GREY;
	vic.color_back = VCOL_MED_GREY;
	vic.color_back1 = VCOL_BLACK;
	vic.color_back2 = VCOL_WHITE;

	// Set VIC to show custom screen with custom charset and multicolor
	vic_setmode(VICM_TEXT_MC, Screen, Charset);

	// Current offset in tilemap
	char tx = 0, ty = 0;
	// Loop forever
	while (true)
	{
		// Wait for first raster line below the first row of tiles
		// to avoid tearing
		vic_waitLine(50 + 32);

		// Expand tiles
		tile_expand(tx, ty);

		// Wait for char input
		char ch = getch();

		// Move offset
		if (ch == 'D' && tx < 6)
			tx++;
		else if (ch == 'A' && tx > 0)
			tx--;
		else if (ch == 'W' && ty > 0)
			ty--;
		else if (ch == 'S' && ty < 10)
			ty++;
	}

	return 0;
}
