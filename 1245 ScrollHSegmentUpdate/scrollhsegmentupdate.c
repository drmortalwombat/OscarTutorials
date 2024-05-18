#include <c64/vic.h>
#include <c64/memmap.h>
#include <c64/joystick.h>
#include <string.h>
#include <conio.h>

// Global memory regions
char * const CharMap = (char *)0xd000;
char * const ColorMap = (char *)0xd800;

// Two screens for double buffering
char * const Screen0 = (char *)0xc000;
char * const Screen1 = (char *)0xc400;

// The level background data
char LevelMap[24][64];
#pragma align(LevelMap, 256)

// Array with the screens for easy switching
char * const Screens[2] = {Screen0, Screen1};

// Pointer to current screen and backbuffer
char * Screen, * Back;

// Current screen index
char si;

// Pixel and char position of screen in level data
char 	xf, xp;

// Scroll invariants:
//
// The backbuffer consists of two regions, an upper region from
// 0 to xf * 3 and a lower region from xf * 3 to 24.  The scroll invariant
// states that the upper region contains rows from the screen to the
// left and the lower region contains rows from the screen to the right.
// 
// This invariant is maintained by the left and right scrolling routines.

void display_init(void)
{
	// Copy charrom to have some charsets
	mmap_set(MMAP_CHAR_ROM);
	memcpy(CharMap, CharMap, 2048);
	mmap_set(MMAP_NO_BASIC);

	// Clear screen
	memset(ColorMap, VCOL_LT_BLUE, 1000);
	memset(Screen0, 160, 1000);
	memset(Screen1, 160, 1000);

	// Fill level backbuffer with some cross image
	for(char y=0; y<24; y++)
	{
		for(char x=0; x<64; x++)
		{
			if (!(y & 3))
			{
				if (!(x & 3))
					LevelMap[y][x] = 91;
				else
					LevelMap[y][x] = 64;
			}
			else if (!(x & 3))
				LevelMap[y][x] = 66;
			else if ((x & 7) == (y & 7))
				LevelMap[y][x] = 77;
			else if ((x & 7) == (-y & 7))
				LevelMap[y][x] = 78;
			else
				LevelMap[y][x] = 32;
		}
	}

	// Add ruler to backbuffer
	for(char x=0; x<64; x++)
	{
		LevelMap[ 0][x] = x % 10 + '0';
		LevelMap[23][x] = x % 10 + '0';

		if (x % 10 == 0)
		{
			LevelMap[ 1][x] = x / 10 + '0';
			LevelMap[22][x] = x / 10 + '0';
		}
	}

	// Copy initial frame data
	for(char y=0; y<24; y++)
	{
		memcpy(Screen0 + 40 * y, LevelMap[y] + 0, 40);
		memcpy(Screen1 + 40 * y, LevelMap[y] + 1, 40);
	}

	// Setup initial scroll position
	Screen = Screen0;
	Back = Screen1;
	xf = 0;
	xp = 0;
	si = 0;
	vic_setmode(VICM_TEXT, Screen, CharMap);
	vic.ctrl2 = xf;
}

void scroll_flip(void)
{
	// Flip the screen
	si = 1 - si;
	Back = Screen;
	Screen = Screens[si];
	vic_setmode(VICM_TEXT, Screen, CharMap);
}

// Scroll one pixel to the right
void scroll_right(void)
{
	vic.color_border++;

	// Update three more rows to the left
	char y0 = xf * 3, y1 = y0 + 3;
	for(char y=y0; y<y1; y++)
	{
		// Copy row from the screen to the backbuffer
		memcpy(Back + 40 * y + 1, Screen + 40 * y, 39);
		// Add new column to the left from the level data
		Back[40 * y + 0] = LevelMap[y][(xp - 1) & 63];
	}
	vic.color_border--;

	// Wait for display bottom before changing screen parameters
	vic_waitBottom();

	// Scroll one pixel to the right
	xf = (xf + 1) & 7;
	if (xf == 0)
	{
		// Crossing smooth scroll boundary, flip screen
		scroll_flip();
		// Update char position
		xp = (xp - 1) & 63;
	}
	// Update scroll register
	vic.ctrl2 = xf;
}

// Scroll one pixel to the left
void scroll_left(void)
{
	// Wait for display bottom before changing screen parameters
	vic_waitBottom();

	// Scroll one pixel to the left
	xf = (xf - 1) & 7;
	if (xf == 7)
	{
		// Crossing smooth scroll boundary, flip screen
		scroll_flip();
		// Update char position
		xp = (xp + 1) & 63;
	}
	// Update scroll register
	vic.ctrl2 = xf;

	vic.color_border++;

	// Update three more rows to the right
	char y0 = xf * 3, y1 = y0 + 3;
	for(char y=y0; y<y1; y++)
	{
		// Copy row from the screen to the backbuffer
		memcpy(Back + 40 * y, Screen + 40 * y + 1, 39);
		// Add new column to the right from the level data
		Back[40 * y + 39] = LevelMap[y][(xp + 40) & 63];
	}
	vic.color_border--;
}

int main(void)
{
	mmap_trampoline();

	display_init();

	for(;;)
	{
		joy_poll(0);

		if (joyx[0] > 0)
			scroll_left();
		else if (joyx[0] < 0)
			scroll_right();
	}
	return 0;
}
