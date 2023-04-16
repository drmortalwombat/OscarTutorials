#include <c64/vic.h>
#include <stdlib.h>

char * const Screen = (char *)0x0400;

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

void fill_line(void)
{
	for(char i=0; i<40; i++)
		Screen[40 * 24 + i] = (rand() & 1) + 77;
}

int main(void)
{
	for(;;)
	{
		vic_waitBottom();

		vic.color_border = 0;
		scroll_up();
		vic.color_border = 1;
		fill_line();
		vic.color_border = 2;
	}

	return 0;
}
