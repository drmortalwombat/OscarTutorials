#include <c64/vic.h>

int main(void)
{
	for(;;)
	{
		// Change border and background color
		vic.color_border = VCOL_BLACK;
		vic.color_back = VCOL_BLACK;

		// Wait for raster to leave screen
		while (!(vic.ctrl1 & VIC_CTRL1_RST8))
			;

		// Change border and background color
		vic.color_border = VCOL_WHITE;
		vic.color_back = VCOL_WHITE;

		// Wait for raster to reenter screen
		while (vic.ctrl1 & VIC_CTRL1_RST8)
			;
	}

	return 0;
}
