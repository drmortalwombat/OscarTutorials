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

// Scroll one char up
void scroll_up(void)
{
	for(char i=0; i<40; i++)
	{
		#pragma unroll(full)
		for(char j=0; j<12; j++)
			Screen[40 * j + i] = Screen[40 * (j + 1) + i];
	}
	for(char i=0; i<40; i++)
	{
		#pragma unroll(full)
		for(char j=12; j<24; j++)
			Screen[40 * j + i] = Screen[40 * (j + 1) + i];
	}
}

// Scroll one char left
void scroll_left(void)
{
	for(char i=0; i<39; i++)
	{
		#pragma unroll(full)
		for(char j=0; j<12; j++)
			Screen[40 * j + i] = Screen[40 * j + i + 1];
	}
	for(char i=0; i<39; i++)
	{
		#pragma unroll(full)
		for(char j=12; j<25; j++)
			Screen[40 * j + i] = Screen[40 * j + i + 1];
	}
}

// Scroll one char right
void scroll_right(void)
{
	for(char i=39; i>0; i--)
	{
		#pragma unroll(full)
		for(char j=0; j<12; j++)
			Screen[40 * j + i] = Screen[40 * j + i - 1];
	}
	for(char i=39; i>0; i--)
	{
		#pragma unroll(full)
		for(char j=12; j<25; j++)
			Screen[40 * j + i] = Screen[40 * j + i - 1];
	}
}

// Scroll one char down
void scroll_down(void)
{
	char buffer[40];

	for(char i=0; i<40; i++)
		buffer[i] = Screen[40 * 12 + i];

	for(char i=0; i<40; i++)
	{
		#pragma unroll(full)
		for(char j=12; j>0; j--)
			Screen[40 * j + i] = Screen[40 * (j - 1) + i];
	}
	for(char i=0; i<40; i++)
	{
		#pragma unroll(full)
		for(char j=24; j>13; j--)
			Screen[40 * j + i] = Screen[40 * (j - 1) + i];
	}

	for(char i=0; i<40; i++)
		Screen[40 * 13 + i] = buffer[i];
}

// Fill line with new data
void fill_line(char y, char sx, char sy)
{
	// Target screen pointer
	char * sp = Screen + 40 * y;
	for(char x=0; x<40; x++)
	{
		// Start of the tilemap, indexed by the offset coordinate
		char	tile = PipeMap[(sx >> 2) + 16 * (sy >> 2)];
		const char	*	tp = PipTiles + 16 * tile;
		sp[x] = tp[(sx & 3) + 4 * (sy & 3)];
		sx++;
	}
}

// Fill column with new data
void fill_column(char x, char sx, char sy)
{
	// Target screen pointer
	char * sp = Screen + x;
	for(char y=0; y<25; y++)
	{
		// Start of the tilemap, indexed by the offset coordinate
		char	tile = PipeMap[(sx >> 2) + 16 * (sy >> 2)];
		const char	*	tp = PipTiles + 16 * tile;
		sp[0] = tp[(sx & 3) + 4 * (sy & 3)];
		sp += 40;
		sy++;
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

	// Position in pixel
	unsigned ix = 0, iy = 0;

	for(char i=0; i<25; i++)
		fill_line(i, 0, i);

	// Velocity in pixel per frame
	signed char vx = 1, vy = 0;

	// Initial fine scroll values
	vic.ctrl1 = VIC_CTRL1_DEN | ((iy & 7) ^ 7);
	vic.ctrl2 = ((ix & 7) ^ 7) | VIC_CTRL2_MCM;

	for(;;)
	{
		// Check if user pressed key
		char ch = getchx();

		// Set direction based on input
		switch (ch)
		{
		case 17:		// Down
			vx = 0; vy = 1;
			break;
		case 17 + 128:	// Up
			vx = 0; vy = -1;
			break;
		case 29:		// Right
			vx = 1; vy = 0;
			break;
		case 29 + 128:	// Left
			vx = -1; vy = 0;
			break;
		}

		// Wait for raster to reach bottom of screen
		vic_waitBottom();

		if (vx < 0)
		{
			// Scroll right
			if (ix > 0)
			{
				ix--;
				vic.ctrl2 = ((ix & 7) ^ 7) | VIC_CTRL2_MCM;

				if ((ix & 7) == 7)
				{
					// Scroll by char
					scroll_right();
					// Fill new column
					fill_column(0, ix >> 3, iy >> 3);
				}
				else
					vic_waitTop();
			}
		}
		else if (vx > 0)
		{
			// Scroll left
			if (ix < 6 * 4 * 8)
			{
				ix++;
				vic.ctrl2 = ((ix & 7) ^ 7) | VIC_CTRL2_MCM;

				if ((ix & 7) == 0)
				{
					// Scroll by char
					scroll_left();
					// Fill new column
					vic.color_border++;
					fill_column(39, (ix >> 3) + 39, iy >> 3);
					vic.color_border--;
				}
				else
					vic_waitTop();
			}
		}
		else if (vy < 0)
		{
			// Scroll down
			if (iy > 0)
			{
				iy--;
				vic.ctrl1 = VIC_CTRL1_DEN | ((iy & 7) ^ 7);

				if ((iy & 7) == 7)
				{
					// Scroll by char
					scroll_down();
					// Fill new row
					fill_line(0, ix >> 3, iy >> 3);
				}
				else
					vic_waitTop();
			}
		}
		else if (vy > 0)
		{
			// Scroll up
			if (iy < 10 * 4 * 8)
			{
				iy++;
				vic.ctrl1 = VIC_CTRL1_DEN | ((iy & 7) ^ 7);

				if ((iy & 7) == 0)
				{
					// Scroll by char
					scroll_up();
					// Fill new row
					vic.color_border++;
					fill_line(24, ix >> 3, (iy >> 3) + 24);
					vic.color_border--;
				}
				else
					vic_waitTop();
			}
		}
	}

	return 0;
}
