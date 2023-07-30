#include <c64/vic.h>
#include <string.h>

// Make room for second hires color buffer from 8c00.8fff
#pragma region( main, 0x0900, 0x8c00, , , {code, data, bss, heap} )


// The two bitmap buffers
static char * const Hires1 = (char *)0xe000;
static char * const Hires2 = (char *)0xa000;

// The two color buffers
static char * const Color1 = (char *)0xc000;
static char * const Color2 = (char *)0x8c00;

// Color RAM
static char * const ColorR = (char *)0xd800;

// Source image hires part
const char SrcHires[] = {
	#embed 19200 0 "../Resources/mchiresscroll.bin"	
};

// Source image first color part
const char SrcColor0[] = {
	#embed 2400 19200 "../Resources/mchiresscroll.bin"
};

// Source image second color part
const char SrcColor1[] = {
	#embed 2400 21600 "../Resources/mchiresscroll.bin"
};

// Scroll up one char row
void scroll(unsigned n, char * hp, char * cp)
{
	// Source data pointers
	const char * hdata = SrcHires + 8 * n;
	const char * cdata = SrcColor0 + n;
	const char * rdata = SrcColor1 + n;

	// Hires pointers to copy in chunks of 1600 bytes
	char * dp = hp;
	const char * sp = hdata;

	// Copy five chunks, one for each raster line, takes more
	// than one frame, so we get a one line every to frames
	// scroll speed
	for(int j=0; j<5; j++)
	{
		// copy chunk of hires data
		memcpy(dp, sp, 1600);
		dp += 1600;
		sp += 1600;

		// sync with the raster
		vic_waitBottom();
		// scroll one line
		vic.ctrl1 = VIC_CTRL1_BMM | VIC_CTRL1_DEN | (6 - j);
	}

	// copy color 0 data, takes ~1 frame
	memcpy(cp, cdata, 1000);
	vic_waitBottom();
	vic_waitTop();

	// sync with raster, and scroll to next line
	vic_waitBottom();
	vic.ctrl1 = VIC_CTRL1_BMM | VIC_CTRL1_DEN | 1;

	// final raster scroll with no work to do
	vic_waitTop();
	vic_waitBottom();
	vic_waitTop();
	vic_waitBottom();
	vic.ctrl1 = VIC_CTRL1_BMM | VIC_CTRL1_DEN | 0;

	// and now the tricky part, copying the color buffer
	vic_waitTop();
	vic_waitBottom();

	// wait for top of screen, so we have max time to copy
	vic_waitLine(50);

	// copy color buffer
	memcpy(ColorR, rdata, 1000);

	// and toggle double buffer
	vic_setmode(VICM_HIRES_MC, cp, hp);
	vic.ctrl1 = VIC_CTRL1_BMM | VIC_CTRL1_DEN | 7;
}

int main(void)
{
	// border and background
	vic.color_back = VCOL_BLACK;
	vic.color_border = VCOL_BLACK;

	// start with a blank screen
	memset(Hires2, 0, 8000);
	memset(Color2, 0, 1000);
	memset(ColorR, 0, 1000);
	vic_setmode(VICM_HIRES_MC, Color2, Hires2);

	__asm { sei }

	// loop through image in 40 (320) byte chunks
	unsigned	n = 0;	
	for(int i=0; i<280; i+=16)
	{
		// First frame
		scroll(n, Hires1, Color1);
		n += 40;

		// Second frame
		scroll(n, Hires2, Color2);
		n += 40;
	}

	__asm { cli }

	return 0;
}
