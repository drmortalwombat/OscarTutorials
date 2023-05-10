#include <c64/vic.h>
#include <c64/memmap.h>
#include <math.h>
#include <string.h>

char * const Hires	= (char *)0xe000;
char * const Screen	= (char *)0xd000;

void set(int x, int y)
{
	if (x >= 0 && x < 320 && y >= 0 && y < 200)
	{
		char	*	dp = Hires + 320 * (y >> 3) + (y & 7) + (x & ~7);
		*dp |= 0x80 >> (x & 7);
	}
}

int main(void)
{
	mmap_trampoline();
	mmap_set(MMAP_RAM);
	memset(Screen, 0x01, 1000);
	memset(Hires, 0x00, 8000);
	mmap_set(MMAP_NO_ROM);

	vic_setmode(VICM_HIRES, Screen, Hires);

	float	w = 0.0;

	for(;;)
	{
		int x = 160 + cos(w) * 80 + cos(w * 5) * 20 + cos(w * 13) * 10;
		int y = 100 + sin(w * 2) * 80 + sin(w * 7) * 20 + sin(w * 11) * 10;

		set(x, y);

		w += 0.01;
	}

	return 0;
}
