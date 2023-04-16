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
	char i = 0;
	for(;;)
	{
		vic_waitBottom();
		i++;
		if (i == 8)
		{	
			i = 0;
			vic.ctrl1 = VIC_CTRL1_DEN | (i ^ 7);

			scroll_up();
			fill_line();
		}
		else
		{
			vic.ctrl1 = VIC_CTRL1_DEN | (i ^ 7);
			vic_waitTop();
		}
	}

	return 0;
}
