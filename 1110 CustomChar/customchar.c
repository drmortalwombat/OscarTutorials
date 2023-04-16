#include <string.h>
#include <stdlib.h>
#include <c64/vic.h>

char * const Screen = (char *)0xc000;
char * const Charset = (char *)0xc800;
char * const Color = (char *)0xd800;

const char Smiley[] = {
	0b01111100,
	0b11111110,
	0b10010010,
	0b11111110,
	0b10000010,
	0b11000110,
	0b01111100,
	0b00000000,

	0b01111100,
	0b11111110,
	0b10010010,
	0b11111110,
	0b11000110,
	0b10111010,
	0b01111100,
	0b00000000,
	
};

int main(void)
{
	memcpy(Charset, Smiley, sizeof(Smiley));

	for(unsigned i=0; i<1000; i++)	
		Screen[i] = rand() & 1;
	memset(Color, VCOL_YELLOW, 1000);

	vic.color_border = VCOL_BLACK;
	vic.color_back = VCOL_BLACK;

	vic_setmode(VICM_TEXT, Screen, Charset);

	while (true)
		;

	return 0;
}
