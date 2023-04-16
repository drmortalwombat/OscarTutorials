#include <c64/vic.h>
#include <stdlib.h>

char * const Screen = (char *)0x0400;

void scroll_left(void)
{
	for(char i=0; i<39; i++)
	{
		#pragma unroll(full)
		for(char j=0; j<12; j++)
			Screen[40 * j + i] = Screen[40 * j + i + 1];
	}
	for(char i=0; i<39; i++)
	{
		#pragma unroll(full)
		for(char j=12; j<25; j++)
			Screen[40 * j + i] = Screen[40 * j + i + 1];
	}
}

void fill_column(void)
{
	for(char i=0; i<25; i++)
		Screen[40 * i + 39] = (rand() & 1) + 77;
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
			vic.ctrl2 = (i ^ 7);

			scroll_left();
			fill_column();
		}
		else
		{
			vic.ctrl2 = (i ^ 7);
			vic_waitTop();
		}
	}

	return 0;
}
