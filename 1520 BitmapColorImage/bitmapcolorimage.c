#include <c64/vic.h>
#include <c64/memmap.h>
#include <oscar.h>

char * const Hires	= (char *)0xe000;
char * const Screen	= (char *)0xd000;
char * const Color	= (char *)0xd800;

static const char ImgHires[] = {
	#embed 8000 0 lzo "../Resources/blumba.mcimg"
};

static const char ImgScreen[] = {
	#embed 1000 8000 lzo "../Resources/blumba.mcimg"
};

static const char ImgColor[] = {
	#embed 1000 9000 lzo "../Resources/blumba.mcimg"
};

int main(void)
{
	mmap_trampoline();
	mmap_set(MMAP_RAM);

	oscar_expand_lzo(Hires, ImgHires);
	oscar_expand_lzo(Screen, ImgScreen);

	mmap_set(MMAP_ROM);

	oscar_expand_lzo(Color, ImgColor);

	vic_setmode(VICM_HIRES_MC, Screen, Hires);
	vic.color_border = VCOL_BLACK;
	vic.color_back = VCOL_BLACK;

	for(;;) 
		;

	return 0;
}
