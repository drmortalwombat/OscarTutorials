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

char tile_row[11];

// Generator function for filling a full charrow of tiles.  This will be inlined expanded
// with a constant rx to generate the four variants with x offset of 0 to 3 into
// the tile

inline void fill_line_xy(char * sp, char rx, char ry)
{
	// Simple version where the tile offset is zero, so the tiles
	// match the screen position

	// Get first tile
	char tile = tile_row[0];

	// Full unroll for static addressing
	#pragma unroll(full)
	for(char x=0; x<10; x++)
	{

		// Full unroll for static addressing
		#pragma unroll(full)
		for(char i=0; i<4; i++)
		{
			// Copy one char
			*sp++ = PipeRetiled[tile + ((rx + i) & 3) * 16 + ry * 64];

			// Advance to next tile, when at end
			if (rx + i == 3)
				tile = tile_row[x + 1];
		}
	}
}

// Fill a single char line of a tile with offset ry and rx
// into the tile.  The inlining is used to get constant
// expanded variants for ry

inline void fill_line_x(char * sp, char rx, char ry)
{
	switch(rx)
	{
	case 0:
		fill_line_xy(sp, 0, ry);
		break;
	case 1:
		fill_line_xy(sp, 1, ry);
		break;
	case 2:
		fill_line_xy(sp, 2, ry);
		break;
	case 3:
		fill_line_xy(sp, 3, ry);
		break;
	}
}


// Generator function for filling a full row of tiles.  This will be inlined expanded
// with a constant rx to generate the four variants with x offset of 0 to 3 into
// the tile

inline void fill_qline_xy(char * sp0, char * sp1, char * sp2, char * sp3, char rx)
{
	// Simple version where the tile offset is zero, so the tiles
	// match the screen position

	char tile = tile_row[0];

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

			// Fetch the next tile, when at last
			if (rx + i == 3)
				tile = tile_row[x + 1];
		}
	}
}


// Fill a full row of tiles (four rows of char) using
// four individual screen pointers, each 40 bytes appart
// to allow dense y indexing.

void fill_qline_x(char * sp0, char * sp1, char * sp2, char * sp3, char rx)
{
	// Expand variants for x offset
	switch(rx)
	{
	case 0:
		fill_qline_xy(sp0, sp1, sp2, sp3, 0);
		break;
	case 1:
		fill_qline_xy(sp0, sp1, sp2, sp3, 1);
		break;
	case 2:
		fill_qline_xy(sp0, sp1, sp2, sp3, 2);
		break;
	case 3:
		fill_qline_xy(sp0, sp1, sp2, sp3, 3);
		break;
	}
}


// Four functions for the variable y offset start in the tile, the
// inline of the fill_line_x will produce expanded code, with the
// ry parameter replaced by a constant

void fill_line_y0(char * sp, char rx)
{
	fill_line_x(sp, rx, 0);
}

void fill_line_y1(char * sp, char rx)
{
	fill_line_x(sp, rx, 1);
}

void fill_line_y2(char * sp, char rx)
{
	fill_line_x(sp, rx, 2);
}

void fill_line_y3(char * sp, char rx)
{
	fill_line_x(sp, rx, 3);
}


void fill_screen(char sx, char sy)
{
	// Target screen pointer
	char * sp = Screen;

	// Tile map source pointer
	const char * tm = PipeMap + (sy >> 2) * 16 + (sx >> 2);

	// We only need the offset from here on
	sx &= 3;

	// Copy first row of tiles into fixed buffer
	#pragma unroll(full)
	for(char i=0; i<11; i++)
		tile_row[i] = tm[i];
	tm += 16;

	// Copy first incomplete row of tiles based
	// on y offset
	char y = 0;
	switch (sy & 3)
	{
	case 1:
		fill_line_y1(sp, sx);
		sp += 40;
		y++;
		// Intentional fall through
	case 2:
		fill_line_y2(sp, sx);
		sp += 40;
		y++;
		// Intentional fall through
	case 3:
		fill_line_y3(sp, sx);
		sp += 40;
		y++;

		// Fetch next row of tiles
		#pragma unroll(full)
		for(char i=0; i<11; i++)
			tile_row[i] = tm[i];
		tm += 16;
	}

	while (y < 22)
	{
		// Expand complete rows of tiles
		fill_qline_x(sp, sp + 40, sp + 80, sp + 120, sx);
		sp += 160;
		y += 4;	

		// Fetch next row of tiles
		#pragma unroll(full)
		for(char i=0; i<11; i++)
			tile_row[i] = tm[i];
		tm += 16;
	}

	// Expand remaining incomplete row of tiles
	switch (y)
	{
	case 22:
		fill_line_y2(sp + 80, sx);
	case 23:
		fill_line_y1(sp + 40, sx);
	case 24:
		fill_line_y0(sp, sx);
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
		if (ix < 0 || ix > 6 * 4 * 8)
		{
			vx = -vx;
			ix += vx;
		}
		if (iy < 0 || iy > 10 * 4 * 8)
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