#include <stdio.h>
#include <complex.h>
#include <math.h>
#include <time.h>

#define NUMBER 100000
#define M_PI 3.14159265358979323846

void timespec_diff(struct timespec *start, struct timespec *stop, struct timespec *result)
{
    if ((stop->tv_nsec - start->tv_nsec) < 0)
    {
        result->tv_sec = stop->tv_sec - start->tv_sec - 1;
        result->tv_nsec = stop->tv_nsec - start->tv_nsec + 1000000000;
    }
    else
    {
        result->tv_sec = stop->tv_sec - start->tv_sec;
        result->tv_nsec = stop->tv_nsec - start->tv_nsec;
    }
    return;
}

static inline double complex compute(double complex z, double d, int c) {
	double real, imag, add_for_correction, r, theta;
	double complex res;
	
	for(int i = c; i > 0; i--) {
		real = creal(z);
		imag = cimag(z);
		add_for_correction = 0;
	
		if(real < 0) add_for_correction = M_PI;
		else if (imag < 0) add_for_correction = 2 * M_PI;
	
		r = sqrt(real * real + imag *imag);
		theta = atan(imag / real) + add_for_correction;
		
		res = z - ( ((pow(r, d) * (cos(d * theta) + I * sin(d * theta)))-1) / (d*(pow(r, (d-1)) * (cos((d-1) * theta) + I * sin((d-1) * theta)))) );
		z = res;
	}
	
	return res;
}


int main() {
    struct timespec start;
    struct timespec end;
    struct timespec diff, diff1;

    double complex z = 1.0 + 1.0 * I;
    double d = 5.0;
    double complex res;
    int c = 12;
	
    timespec_get(&start, TIME_UTC);
    for (size_t i = 0; i < 1; ++i) {
        res = compute(z, d, c);
    }
	
    timespec_get(&end, TIME_UTC);
    timespec_diff(&start, &end, &diff);

    printf("Result 1 :\t%.10f%+.10fi\n", creal(res), cimag(res));
    printf("\n");
    printf("Time 1 :\t%lis and %fms\n", diff.tv_sec, (diff.tv_nsec / 1000000.0));
	
	return 0;
}
