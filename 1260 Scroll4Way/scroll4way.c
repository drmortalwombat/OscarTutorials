#include <c64/vic.h>
#include <stdlib.h>
#include <conio.h>

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

void scroll_right(void)
{
	for(char i=39; i>0; i--)
	{
		#pragma unroll(full)
		for(char j=0; j<12; j++)
			Screen[40 * j + i] = Screen[40 * j + i - 1];
	}
	for(char i=39; i>0; i--)
	{
		#pragma unroll(full)
		for(char j=12; j<25; j++)
			Screen[40 * j + i] = Screen[40 * j + i - 1];
	}
}

void scroll_down(void)
{
	char buffer[40];

	for(char i=0; i<40; i++)
		buffer[i] = Screen[40 * 12 + i];

	for(char i=0; i<40; i++)
	{
		#pragma unroll(full)
		for(char j=12; j>0; j--)
			Screen[40 * j + i] = Screen[40 * (j - 1) + i];
	}
	for(char i=0; i<40; i++)
	{
		#pragma unroll(full)
		for(char j=24; j>13; j--)
			Screen[40 * j + i] = Screen[40 * (j - 1) + i];
	}

	for(char i=0; i<40; i++)
		Screen[40 * 13 + i] = buffer[i];
}

void fill_line(char y)
{
	for(char i=0; i<40; i++)
		Screen[40 * y + i] = (rand() & 1) + 77;
}

void fill_column(char x)
{
	for(char i=0; i<25; i++)
		Screen[40 * i + x] = (rand() & 1) + 77;
}

int main(void)
{
	signed char ix = 0, iy = 0;
	signed char vx = 1, vy = 0;

	for(;;)
	{
		vic_waitBottom();

		char ch = getchx();
		switch (ch)
		{
		case 17:		// Down
			vx = 0; vy = 1;
			break;
		case 17 + 128:	// Up
			vx = 0; vy = -1;
			break;
		case 29:		// Right
			vx = 1; vy = 0;
			break;
		case 29 + 128:	// Left
			vx = -1; vy = 0;
			break;
		}

		ix += vx;
		iy += vy;

		if (ix < 0)
		{
			ix += 8;
			vic.ctrl2 = ix;
			scroll_left();
			fill_column(39);			
		}
		else if (ix >= 8)
		{
			ix -= 8;
			vic.ctrl2 = ix;
			scroll_right();
			fill_column(0);
		}
		else if (iy < 0)
		{
			iy += 8;
			vic.ctrl1 = VIC_CTRL1_DEN | iy;
			scroll_up();
			fill_line(24);
		}
		else if (iy >= 8)
		{
			iy -= 8;
			vic.ctrl1 = VIC_CTRL1_DEN | iy;
			scroll_down();
			fill_line(0);
		}
		else
		{
			vic.ctrl1 = VIC_CTRL1_DEN | iy;
			vic.ctrl2 = ix;
			vic_waitTop();
		}
	}

	return 0;
}
