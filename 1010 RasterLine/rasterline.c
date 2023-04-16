#include <c64/vic.h>

int main(void)
{
	for(;;)
	{
		vic.color_border = vic.raster >> 4;
	}

	return 0;
}
