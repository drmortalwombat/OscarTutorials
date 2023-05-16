#include <stdio.h>
#include <math.h>
#include <time.h>

// Error function for reaching target vector of (1.0, 0.0) at end of
// iterations
float ferror(float x, float y)
{
	return (x - 1.0) * (x - 1.0) + y * y;
}

int main(void)
{
	// Declare float variables
	float	fx = 1.0, fy = 0.0;

	static const int N = 200;

	// Float constant for step, N th of the circumference
	static const float	ds = 2.0 * PI / N;

	clock_t	t0 = clock();

	// Loop N interations for full circle
	for(int i=0; i<N; i++)
	{
		// Direction to move in small steps in a circle
		float dx = ds * -fy, dy = ds * fx;

		// Advance by step
		fx += dx; fy += dy;
	}

	clock_t	t1 = clock();

	// Print result
	printf("V: %6.3f %6.3f  E: %6.3f  T: %5.4f\n", fx, fy, ferror(fx, fy), (t1 - t0) / 60.0);

	return 0;
}
