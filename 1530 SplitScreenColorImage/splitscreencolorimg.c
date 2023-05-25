#include <c64/vic.h>
#include <c64/memmap.h>
#include <c64/rasterirq.h>
#include <string.h>
#include <oscar.h>

// Place bitmap memory underneath kernal ROM
char * const Hires	= (char *)0xe000;

// Place cell color map in normal RAM
char * const Screen	= (char *)0xc000;
char * const Color	= (char *)0xd800;

// Copy of font from ROM to RAM
char * const Font	= (char *)0xd000;

// Compressed bitmap image and color data
static const char ImgHires[] = {
	#embed 8000 0 lzo "../Resources/blumba.mcimg"
};

static const char ImgScreen[] = {
	#embed 1000 8000 lzo "../Resources/blumba.mcimg"
};

static const char ImgColor[] = {
	#embed 1000 9000 lzo "../Resources/blumba.mcimg"
};

const char * Text =
	S"LABORUM RERUM QUO. QUASI IN, SEQUI, TENETUR VOLUPTATEM RERUM "
	S"PORRO NON ET MAIORES ALIAS ODIO EST EOS. MAGNAM APERIAM CUM ET "
	S"ESSE TEMPORE ITAQUE TEMPORA VOLUPTAS ET IPSAM IPSAM EARUM. ID "
	S"SUSCIPIT QUIA RERUM REPREHENDERIT ERROR ET UT. DOLOR ID "
	S"CORPORIS, EOS? UNDE VERO ISTE QUIA? EAQUE EAQUE. IN. AUT ID "
	S"EXPEDITA ILLUM MOLESTIAS, ";

// Raster interrupt command structure for change to scrolled and back

RIRQCode	scroll, bottom;

int main(void)
{
	// Install trampoline to keep system running with ROM paged out
	mmap_trampoline();

	// Page in RAM
	mmap_set(MMAP_CHAR_ROM);

	// Expand hires and first color map
	oscar_expand_lzo(Hires, ImgHires);
	oscar_expand_lzo(Screen, ImgScreen);

	// Copy system font into RAM
	memcpy(Font, Font, 2048);

	// Turn IO space back on
	mmap_set(MMAP_ROM);

	// Expand color attributes
	oscar_expand_lzo(Color, ImgColor);

	// Set hires multicolor mode
	vic_setmode(VICM_HIRES_MC, Screen, Hires);
	vic.color_border = VCOL_BLACK;
	vic.color_back = VCOL_BLACK;

	// Init the raster IRQ system to use the kernal iterrupt vector
	rirq_init_kernal();

	// Build switch to scroll line IRQ
	rirq_build(&scroll, 4);
	// Wait until end of line
	rirq_delay(&scroll, 10);
	// Switch to text mode
	rirq_write(&scroll, 1, &vic.ctrl1, VIC_CTRL1_DEN | VIC_CTRL1_RSEL | 3);
	// Change control register two with this IRQ
	rirq_write(&scroll, 2, &vic.ctrl2, 0);
	// Memory map for font
	rirq_write(&scroll, 3, &vic.memptr, 0x04);
	// Put it onto the scroll line
	rirq_set(0, 48 + 24 * 8, &scroll);

	// Build the switch to normal IRQ
	rirq_build(&bottom, 3);
	// restore hires mode
	rirq_write(&bottom, 0, &vic.ctrl1, VIC_CTRL1_DEN | VIC_CTRL1_BMM | VIC_CTRL1_RSEL | 3);
	// re-enable left and right column and reset horizontal scroll
	rirq_write(&bottom, 1, &vic.ctrl2, VIC_CTRL2_CSEL | VIC_CTRL2_MCM);
	// restore memory address
	rirq_write(&bottom, 2, &vic.memptr, 0x08);
	// place this at the bottom
	rirq_set(1, 250, &bottom);

	// sort the raster IRQs
	rirq_sort();

	// start raster IRQ processing
	rirq_start();

	// Add some color flow for the scroll area
	Color[40 * 24 +  0] = VCOL_DARK_GREY;
	Color[40 * 24 +  1] = VCOL_MED_GREY;
	Color[40 * 24 +  2] = VCOL_LT_GREY;
	memset(Color + 40 * 24 + 3, VCOL_WHITE, 33);
	Color[40 * 24 + 36] = VCOL_LT_GREY;
	Color[40 * 24 + 37] = VCOL_MED_GREY;
	Color[40 * 24 + 38] = VCOL_DARK_GREY;

	// Loop through text
	int	x = 0;
	for(;;)
	{
		// wait for raster reaching bottom of screen
		rirq_wait();
		// Update raster IRQ for scroll line with new horizontal scroll offset		
		rirq_data(&scroll, 2, 7 - (x & 7));
		// Copy scrolled version of text when switching over char border
		if ((x & 7) == 0)
			memcpy((char *)Screen + 40 * 24, Text + ((x >> 3) & 255), 40);
		x++;
	}

	return 0;
}
