#include <c64/vic.h>
#include <stdlib.h>
#include <string.h>

char * const Screen = (char *)0x0400;

void scroll_up(void)
{
	memmove(Screen, Screen + 40, 960);
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
