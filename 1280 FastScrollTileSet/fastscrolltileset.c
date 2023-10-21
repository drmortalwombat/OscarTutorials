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

#define GRID_WIDTH	30
#define GRID_HEIGHT	30

// The charpad resource in lz compression, without the need
// for binary export
const char PipeFont[] = {
	#embed ctm_chars lzo "../Resources/pipes30.ctm"
};

// The tile map, a 16x16 grid of 8 bit tile indices
const char PipeMap[] = {
	#embed ctm_map8 "../Resources/pipes30.ctm"
};

// The tiles, an array of 4x4 char indices
const char PipTiles[] = {
	#embed ctm_tiles8 "../Resources/pipes30.ctm"
};

char PipeRetiled[16 * 4 * 4];

// Remap tiles from an [i][y][x] layout to an [y][x][i] layout to
// allow for simple indexing with a variable tile id, but constant
// x and y offsets

void tile_remap(void)
{
	for(char i=0; i<16; i++)
		for(char y=0; y<4; y++)
			for(char x=0; x<4; x++)
				PipeRetiled[64 * y + 16 * x + i] = PipTiles[16 * i + 4 * y + x];
}

// Copy ot current row of tiles to allow for absolute addressing,
// keeping the number of required index registers in the copy routines
// small


// Generator function for filling a full row of tiles.  This will be inlined expanded
// with a constant rx to generate the four variants with x offset of 0 to 3 into
// the tile

inline void fill_qline_xy(const char * tr, char * sp0, char * sp1, char * sp2, char * sp3, char rx)
{
	// Fetch first tile index from map
	char tile = tr[0];

	// Full unroll for static addressing
	#pragma unroll(full)
	for(char x=0; x<10; x++)
	{
		// Fill all 16 chars using the four independent screen pointers
		// to get a common y indexing

		// Full unroll for static addressing
		#pragma unroll(full)
		for(char i=0; i<4; i++)
		{
			// Fill a column
			*sp0++ = PipeRetiled[tile + ((rx + i) & 3) * 16 + 0 * 64];
			*sp1++ = PipeRetiled[tile + ((rx + i) & 3) * 16 + 1 * 64];
			*sp2++ = PipeRetiled[tile + ((rx + i) & 3) * 16 + 2 * 64];
			*sp3++ = PipeRetiled[tile + ((rx + i) & 3) * 16 + 3 * 64];

			// Fetch the next tile indes from map, when done with last
			// column
			if (rx + i == 3)
				tile = tr[x + 1];
		}
	}
}


// Fill a full row of tiles (four rows of char) using
// four individual screen pointers, each 40 bytes appart
// to allow dense y indexing.

void fill_qline_x(const char * tm, char * sp0, char * sp1, char * sp2, char * sp3, char rx)
{
	// Expand variants for x offset
	switch(rx)
	{
	case 0:
		fill_qline_xy(tm, sp0, sp1, sp2, sp3, 0);
		break;
	case 1:
		fill_qline_xy(tm, sp0, sp1, sp2, sp3, 1);
		break;
	case 2:
		fill_qline_xy(tm, sp0, sp1, sp2, sp3, 2);
		break;
	case 3:
		fill_qline_xy(tm, sp0, sp1, sp2, sp3, 3);
		break;
	}
}



char scratch[40];

void fill_screen(char sx, char sy)
{
	// Target screen pointer
	char * sp = Screen;

	// Tile map source pointer
	const char * tm = PipeMap + (sy >> 2) * GRID_WIDTH + (sx >> 2);

	// We only need the offset from here on
	sx &= 3;


	// Copy first incomplete row of tiles based
	// on y offset, using the scratch memory to ignore the
	// parts of the tiles, which are above screen

	char y = 0;
	switch (sy & 3)
	{
	case 1:
		fill_qline_x(tm, scratch, sp, sp + 40, sp + 80, sx);
		sp += 120;
		y = 3;
		tm += GRID_WIDTH;
		break;
	case 2:
		fill_qline_x(tm, scratch, scratch, sp, sp + 40, sx);
		sp += 80;
		y = 2;
		tm += GRID_WIDTH;
		break;
	case 3:
		fill_qline_x(tm, scratch, scratch, scratch, sp, sx);
		sp += 40;
		y = 1;
		tm += GRID_WIDTH;
		break;
	}

	while (y < 22)
	{
		// Expand complete rows of tiles
		fill_qline_x(tm, sp, sp + 40, sp + 80, sp + 120, sx);
		sp += 160;
		y += 4;	
		tm += GRID_WIDTH;
	}

	// Expand remaining incomplete row of tiles, again
	// storing overflowing rows into scratch memory
	
	switch (y)
	{
	case 22:
		fill_qline_x(tm, sp, sp + 40, sp + 80, scratch, sx);
		break;
	case 23:
		fill_qline_x(tm, sp, sp + 40, scratch, scratch, sx);
		break;
	case 24:
		fill_qline_x(tm, sp, scratch, scratch, scratch, sx);
		break;
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

	tile_remap();

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

	// Position in pixel
	int ix = 0, iy = 0;

	// Speed in pixel per frame
	signed char vx = 0, vy = 0;

	for(;;)
	{
		// Check if user pressed key
		char ch = getchx();

		// Accelerate based on input
		switch (ch)
		{
		case 17:		// Down
			vy++;
			break;
		case 17 + 128:	// Up
			vy--;
			break;
		case 29:		// Right
			vx++;
			break;
		case 29 + 128:	// Left
			vx--;
			break;
		}

		// Advance scroll position
		ix += vx;
		iy += vy;

		// Check for boundary collision and reflect
		if (ix < 0 || ix > (GRID_WIDTH - 10) * 4 * 8)
		{
			vx = -vx;
			ix += vx;
		}
		if (iy < 0 || iy > (GRID_HEIGHT - 6) * 4 * 8)
		{
			vy = -vy;
			iy += vy;
		}

		// Wait for raster to reach bottom of screen
		vic_waitBottom();

		// Timing feedback
		vic.color_border++;

		// Adjust scroll registers
		vic.ctrl2 = ((ix & 7) ^ 7) | VIC_CTRL2_MCM;
		vic.ctrl1 = VIC_CTRL1_DEN | ((iy & 7) ^ 7);

		// Complete refill of screen, could be limited to cases where
		// actual char position changes
		fill_screen(ix >> 3, iy >> 3);

		vic.color_border--;
	}

	return 0;
}