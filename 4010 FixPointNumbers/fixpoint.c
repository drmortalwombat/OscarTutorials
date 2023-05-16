#include <stdio.h>
#include <math.h>
#include <time.h>

static const int FBITS = 12;
#define FCONST(x)	(int)((x) * (1 << FBITS) + 0.5)
#define FMUL(x, y) (int)((long)(x) * (long)(y) >> FBITS)
#define FFLOAT(x) ((float)(x) / (1 << FBITS))

// Error function for reaching target vector of (1.0, 0.0) at end of
// iterations
float ferror(float x, float y)
{
	return (x - 1.0) * (x - 1.0) + y * y;
}

int main(void)
{
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
		int dx = FMUL(ds, -fy), dy = FMUL(ds, fx);

		// Advance by step
		fx += dx; fy += dy;
	}

	clock_t	t1 = clock();

	// Print result
	printf("V: %6.3f %6.3f  E: %6.3f  T: %5.4f\n", FFLOAT(fx), FFLOAT(fy), ferror(FFLOAT(fx), FFLOAT(fy)), (t1 - t0) / 60.0);

	return 0;
}
