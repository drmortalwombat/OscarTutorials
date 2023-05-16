#include <stdio.h>
#include <math.h>
#include <time.h>
#include <fixmath.h>

static const int FBITS = 12;
#define FCONST(x)	(int)((x) * (1 << FBITS) + 0.5)
#define FFLOAT(x) ((float)(x) / (1 << FBITS))

// Lookup table for squares from 0..255
__striped unsigned sqb[256];

#pragma align(sqb, 256);

inline unsigned bmul(char x, char y)
{
	unsigned x2 = sqb[x], y2 = sqb[y];
	if (x > y)
		return ((unsigned long)(x2 - sqb[x - y]) + (unsigned long)y2) >> 1;
	else
		return ((unsigned long)(y2 - sqb[y - x]) + (unsigned long)x2) >> 1;
}

unsigned lmul(unsigned x, unsigned y)
{
	unsigned	lxy = bmul(x & 0xff, y & 0xff);
	unsigned	mxy = bmul(x & 0xff, y >> 8);
	unsigned	myx = bmul(x >> 8, y & 0xff);
	unsigned	hxy = bmul(x >> 8, y >> 8);

	return 
		((unsigned long)lxy +
		((unsigned long)mxy << 8) +
		((unsigned long)myx << 8) +
		((unsigned long)hxy << 16)) >> FBITS;
}

int lsmul(int x, int y)
{
	if (x < 0)
	{
		if (y < 0)
			return (int)lmul(-x, -y);
		else
			return - (int)lmul(-x, y);
	}
	else if (y < 0)
		return - (int)lmul(x, -y);
	else
		return (int)lmul(x, y);
}

// Error function for reaching target vector of (1.0, 0.0) at end of
// iterations
float ferror(float x, float y)
{
	return (x - 1.0) * (x - 1.0) + y * y;
}

int main(void)
{
	// Initialize square table
	for(unsigned i=0; i<256; i++)
		sqb[i] = i * i;

	// Declare float variables
	int	fx = FCONST(1.0), fy = FCONST(0.0);

	static const int N = 200;

	// Float constant for step, N th of the circumference
	static const int	ds = FCONST(2.0 * PI / N);

	clock_t	t0 = clock();

	// Loop N interations
	for(int i=0; i<N; i++)
	{
		// Direction to move in small steps in a circle
		int dx = lsmul(ds, -fy), dy = lsmul(ds, fx);

		// Advance by step
		fx += dx; fy += dy;
	}

	clock_t	t1 = clock();

	// Print result
	printf("V: %6.3f %6.3f  E: %6.3f  T: %5.4f\n", FFLOAT(fx), FFLOAT(fy), ferror(FFLOAT(fx), FFLOAT(fy)), (t1 - t0) / 60.0);

	return 0;

}
