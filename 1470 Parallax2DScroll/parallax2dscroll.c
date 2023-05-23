#include <c64/vic.h>
#include <c64/memmap.h>
#include <c64/joystick.h>
#include <oscar.h>
#include <string.h>

// Custom screen address
char * const Screen = (char *)0xc000;

// Custom charset address
char * const Charset = (char *)0xc800;

// Color mem address
char * const Color = (char *)0xd800;

// The charpad resource in lz compression, without the need
// for binary export
const char IslandFont[] = {
	#embed ctm_chars lzo "../Resources/metalisland.ctm"
};

// The tile map, a 80x50 grid of 8 bit char indices
const char IslandMap[] = {
	#embed ctm_map8 "../Resources/metalisland.ctm"
};

// Copy a segment from the larger map onto the scree
// at the given x and y offsets

void copy_screen(char sx, char sy)
{
	// Screen target position	
	char * sp = Screen;

	// We copy five rows in one loop for performance
	const char * mp0 = IslandMap + 80 * sy + sx;
	const char * mp1 = mp0 + 80;
	const char * mp2 = mp1 + 80;
	const char * mp3 = mp2 + 80;
	const char * mp4 = mp3 + 80;

	vic.color_border++;

	// Unroll the outer loop as well
	#pragma unroll(full)
	for(char y=0; y<5; y++)
	{
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
		mp0 += 400;
		mp1 += 400;
		mp2 += 400;
		mp3 += 400;
		mp4 += 400;
	}
	vic.color_border--;
}

// Four Versions of the 8 chars that are used for the parallax
// part of the background.  
char IslandScrollFont[4][2][2][16];

int main(void)
{
	// Install memory trampoline to ensure system
	// keeps running when playing with the PLA
	mmap_trampoline();

	// Expand charset
	oscar_expand_lzo(Charset, IslandFont);

	// Clear screen, prepare color area for multicolor
	memset(Color, VCOL_WHITE + 8, 1000);
	memset(Screen, 0, 1000);

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
	vic.color_back2 = VCOL_MED_GREY;

	// Set VIC to show custom screen with custom charset and multicolor
	vic_setmode(VICM_TEXT_MC, Screen, Charset);

	// Copy initial screen
	copy_screen(0, 0);

	// Position in 8th of a pixel
	int	sx = 0, sy = 0;

	// Velocity in 8th of a pixel per frame
	signed char vx = 0, vy = 0;

	for(;;)
	{
		// Read joystick
		joy_poll(0);

		// Apply friction
		vx -= (vx + 4) >> 3; vy -= (vy + 4) >> 3;

		// Accelerate according to user input
		vx += 16 * joyx[0]; vy += 16 * joyy[0];

		// Integrate velocity
		sx += vx; sy += vy;

		// Bounce when hitting borders
		if (sx < 0 || sx > 320 * 8)
		{
			vx = -vx;
			sx += vx;
		}
		if (sy < 0 || sy > 200 * 8)
		{
			vy = -vy;
			sy += vy;
		}

		// Wait for bottom of screen
		vic_waitBottom();

		// Scroll position in full pixel
		int 	px = sx >> 3, py = sy >> 3;

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

		// Copy screen
		copy_screen(px >> 3, py >> 3);
	}

	return 0;
}
