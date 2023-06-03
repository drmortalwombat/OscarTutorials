#include <c64/vic.h>
#include <c64/memmap.h>
#include <c64/joystick.h>
#include <c64/sprites.h>
#include <oscar.h>
#include <string.h>

// Custom screen address
char * const Screen = (char *)0xc000;

// Custom spriteset address
char * const Sprites = (char *)0xd000;

// Custom charset address
char * const Charset = (char *)0xc800;

// Color mem address
char * const Color = (char *)0xd800;

// The charpad resource in lz compression, without the need
// for binary export
const char IslandFont[] = {
	#embed ctm_chars lzo "../Resources/metalisletiles.ctm"
};

// The tile char map mapping tile indices to blocks of 4x4 char indices
const char IslandTiles[] = {
	#embed ctm_tiles8 "../Resources/metalisletiles.ctm"
};

// The tile map, a 16x64 grid of 8 bit tile indices
const char IslandMap[] = {
	#embed ctm_map8 "../Resources/metalisletiles.ctm"
};

// Ship sprite image
char const SpriteImages[] = {
	#embed spd_sprites lzo "../Resources/vship.spd"
};

// Temporary buffer for expanded tiles
char tile_buffer[32][64];
#pragma align(tile_buffer, 256)

// Copy a segment from the larger map onto the scree
// at the given x and y offsets

void copy_screen(char sx, char sy)
{
	// Screen target position	
	char * sp = Screen;

	// We copy five rows in one loop for performance
	// Unroll the outer loop as well
	#pragma unroll(full)
	for(char y=0; y<5; y++)
	{
		const char * mp0 = tile_buffer[sy & 31] + sx;
		sy++;
		const char * mp1 = tile_buffer[sy & 31] + sx;
		sy++;
		const char * mp2 = tile_buffer[sy & 31] + sx;
		sy++;
		const char * mp3 = tile_buffer[sy & 31] + sx;
		sy++;
		const char * mp4 = tile_buffer[sy & 31] + sx;
		sy++;

		// Copy five rows
		for(signed char x=39; x>=0; x--)
		{
			sp[x +   0] = mp0[x];
			sp[x +  40] = mp1[x];
			sp[x +  80] = mp2[x];
			sp[x + 120] = mp3[x];
			sp[x + 160] = mp4[x];
		}

		// Advance
		sp += 200;
	}
}

// Expand a row of tiles into tile buffer
void expand_tiles(char sy, char dy)
{
	// Get source tile row
	const char * sp = IslandMap + 16 * (sy & 63);

	// Get target tile buffer pointers
	char * tp0 = tile_buffer[dy + 0];
	char * tp1 = tile_buffer[dy + 1];
	char * tp2 = tile_buffer[dy + 2];
	char * tp3 = tile_buffer[dy + 3];

	// We have 16 tiles (64 chars) for tile buffer
	for(char tx=0; tx<16; tx++)
	{
		// Get actual tile
		char t = sp[tx];

		// Get tile char map address for this tile
		const char * tp = IslandTiles + t * 16;

		// Expand 4x4 block into tile buffer
		#pragma unroll(full)
		for(char x=0; x<4; x++)
		{
			tp0[x + 4 * tx] = tp[x +  0];
			tp1[x + 4 * tx] = tp[x +  4];
			tp2[x + 4 * tx] = tp[x +  8];
			tp3[x + 4 * tx] = tp[x + 12];
		}
	}
}

// Four Versions of the 8 chars that are used for the parallax
// part of the background.  
char IslandScrollFont[4][2][2][16];

int main(void)
{
	// Install memory trampoline to ensure system
	// keeps running when playing with the PLA
	mmap_trampoline();

	mmap_set(MMAP_RAM);

	// Expand charset
	oscar_expand_lzo(Sprites, SpriteImages);

	mmap_set(MMAP_ROM);

	// Expand charset
	oscar_expand_lzo(Charset, IslandFont);

	// Clear screen, prepare color area for multicolor
	memset(Color, VCOL_WHITE + 8, 1000);
	memset(Screen, 0, 1000);

	spr_init(Screen);

	// We have two sets of background 2x2 chars, one with and
	// one without shadows
	for(char k=0; k<2; k++)
	{
		// Two chars vertical is 16 lines
		for(char j=0; j<16; j++)
		{
			// Get a 16 bit version of the two neighbouring chars
			unsigned u = (Charset[j + 32 * k] << 8) | Charset[j + 16 + 32 * k];

			// Create four scrolled versions
			for(char i=0; i<4; i++)
			{
				// Cache scrolled lines
				IslandScrollFont[i][k][0][j] = u >> 8;
				IslandScrollFont[i][k][1][j] = u & 0xff;

				// Rotate two bits to the left
				u = (u << 2) | (u >> 14);
			}
		}
	}

	// Background and border colors
	vic.color_border = VCOL_LT_BLUE;
	vic.color_back = VCOL_LT_BLUE;
	vic.color_back1 = VCOL_BLACK;
	vic.color_back2 = VCOL_LT_GREY;

	// Set VIC to show custom screen with custom charset and multicolor
	vic_setmode(VICM_TEXT_MC, Screen, Charset);

	// Starting tile offset, scrolling down
	char tiley = 192;

	// Position in 8th of a pixel
	int	sx = 0, sy = tiley << 8;

	// Previous scroll position
	int ppx = 0, ppy = 0;

	// Sprite position
	int spx = 160, spy = 200;

	// Expand initial set of rows into tile buffer
	for(char i=0; i<8; i++)
		expand_tiles(tiley + i, (4 * (tiley + i)) & 31);

	// Copy initial screen
	copy_screen(0, 0);

	// Init ship sprite
	vic.spr_mcolor0 = VCOL_BLACK;
	vic.spr_mcolor1 = VCOL_WHITE;

	spr_set(0, true, spx, spy, 65, VCOL_ORANGE, true, false, false);

	for(;;)
	{
		// Read joystick
		joy_poll(0);

		// Move ship by joystick
		spx += 2 * joyx[0];
		spy += 2 * joyy[0];

		// Clip ship position to screen
		if (spx < 32)
			spx = 32;
		else if (spx > 312)
			spx = 312;
		if (spy < 160)
			spy = 160;
		else if (spy > 220)
			spy = 220;

		// Calculate background scroll offset
		sx = spx * 4;
		sy -= 16;

		// Scroll position in full pixel
		int 	px = sx >> 3, py = sy >> 3;

		// Wait for bottom of screen
		vic_waitBottom();

		// Move ship sprite
		spr_move(0, spx, spy);
		spr_image(0, 65 + joyx[0]);

		// Offset of parallax scrolled regions, half in x due
		// to multicolor mode
		char	ty = ((py >> 1) & 15) ^ 15;
		char	tx = ((px >> 2) &  7) ^ 7;

		// Char and pixel components of x
		char	rx = tx >> 2;
		tx &= 3;

		// Copy the char data from the cached shifted chars
		for(char i=0; i<16; i++)
		{
			Charset[i +  0] = IslandScrollFont[tx][0][1 - rx][ty];
			Charset[i + 16] = IslandScrollFont[tx][0][    rx][ty];

			Charset[i + 32] = IslandScrollFont[tx][1][1 - rx][ty];
			Charset[i + 48] = IslandScrollFont[tx][1][    rx][ty];

			// Next line
			ty = (ty + 1) & 15;
		}
		
		// Set scroll registers
		vic.ctrl1 = VIC_CTRL1_DEN | ((py & 7) ^ 7);
		vic.ctrl2 = VIC_CTRL2_MCM | ((px & 7) ^ 7);

		// Reduce scroll offset to tile offset
		px >>= 3;
		py >>= 3;

		// Check if need to rebuild screen due to scrolling
		if (px != ppx || py != ppy)
		{
			// Copy screen
			copy_screen(px, py);
			ppx = px;
			ppy = py;
		}
		else
		{
			// NO scrolling, check if we need to expand a row of tiles

			char tpy = py >> 2;
			if (tpy <= tiley)
			{
				// Expand row of tiles above screen
				tiley--;
				expand_tiles(tiley, (4 * tiley) & 31);
			}

			// Wait for exit of blank area
			vic_waitTop();
		}
	}

	return 0;
}
