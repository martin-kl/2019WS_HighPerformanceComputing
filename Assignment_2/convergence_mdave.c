#include <stdio.h>
#include <complex.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#define MAX_LINES 10000// the maximum number of lines the program will accept, gives an integer overflow warning when using 100,000
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

static inline double complex compute(double complex z, double d) {
	double real, imag, add_for_correction, r, theta;
	double complex res;
	
	real = creal(z);
	imag = cimag(z);
	add_for_correction = 0;
	
	if(real < 0) add_for_correction = M_PI;
	else if (imag < 0) add_for_correction = 2 * M_PI;
	
	r = sqrt(real * real + imag *imag);
	theta = atan(imag/real) + add_for_correction;
		
	res = z - ( ((pow(r, d) * (cos(d * theta) + I * sin(d * theta)))-1) / (d*(pow(r, (d-1)) * (cos((d-1) * theta) + I * sin((d-1) * theta)))) );
	z = res;
	
	return res;
}

static inline void converge(double d) {

    double complex root[9][9] = {{1,0,0,0,0,0,0,0,0},
	{1,-1,0,0,0,0,0,0,0},
	{1,-0.5 - (0.8660254037)*I,-0.5 + 0.8660254037*I,0,0,0,0,0,0},
	{1,-1,I,-I,0,0,0,0,0},
	{1,-0.8090169943 - 0.5877852522*I,-0.8090169943 + 0.5877852522*I,0.3090169943 - 0.9510565162*I,0.3090169943 + 0.9510565162*I,0,0,0,0},
	{1,-1,-0.5 + 0.8660254037*I,0.5 - 0.8660254037*I,-0.5 - 0.8660254037*I,0.5 + 0.8660254037*I,0,0,0},
	{1,-0.9009688679 - 0.4338837391*I,0.6234898018 + 0.7818314824*I,-0.2225209339 - 0.9749279121*I,-0.2225209339 + 0.9749279121*I,0.6234898018 - 0.7818314824*I,-0.9009688679 + 0.4338837391*I,0,0},
	{1,-1,I,-I,0.7071067811 + 0.7071067811*I,-0.7071067811 - 0.7071067811*I,0.7071067811 - 0.7071067811*I,-0.7071067811 + 0.7071067811*I,0},
	{1,-0.9396926207 - 0.3420201433*I,0.7660444431 + 0.6427876096*I,-0.5 - 0.8660254037*I,0.1736481776 + 0.9848077530*I,0.1736481776 - 0.9848077530*I,-0.5 + 0.8660254037*I,0.7660444431 - 0.6427876096*I,-0.9396926207 + 0.3420201433*I}
	};

	double complex x1;
	double complex diff;
	double complex **x0 = (double complex**)malloc(1000*1000*1000*sizeof(double complex));
	double complex *x0_entries = (double complex*)malloc(1000*1000*sizeof(double complex));
	
	int size = 1000;
	
	for(int i = 0, j = 0; i < size; i++) {
		x0[i] = x0_entries + j;
		j += size;
	}
	
	int c = 0, r = 0; 
	
	int checkCompute = -1;
	int iterations = 0;
	
	// calc only for x:[-2 2], y:[0 2]
	
	for(double i = -0.5; i <= +0.5; i=i+0.004) {
		for(double j = -0.5; j <= 0.5; j=j+0.004) {
			x0[r][c] = i + j*I;
			c = c++;

			checkCompute = -1;
			
			if( (r == 0) && (c == 0))
				x1 = x0[r][c];

			for(iterations = 0; checkCompute != 0; iterations++) {
				if( (creal(x1)*creal(x1) + cimag(x1)*cimag(x1)) < 0.000001 ) {
					checkCompute = 0;
					break;
				}
				
				
				if( (creal(x1) > 10000000000) || (cimag(x1) > 10000000000) || (creal(x1) < -10000000000) || (cimag(x1) < -10000000000) ) {
					checkCompute = 0;
					break;
				}
				
				int k = 0;
				while( k < d ) {
					diff = x1 - root[((int)d)-1][k];
					//printf("%.10f\t%.10f\t%.10f\n", creal(diff), creal(x1), creal(root[((int)d)-1][k]));
					//printf("%.10f\t%.10f\t%.10f\n", cimag(diff), cimag(x1), cimag(root[((int)d)-1][k]));
					//printf("dist: %.10f\n\n", (creal(diff)*creal(diff) + cimag(diff)*cimag(diff)));
					
					if ( (creal(diff)*creal(diff) + cimag(diff)*cimag(diff)) < 0.000001) {
						checkCompute = 0;
						break;
					}
					k=k+1;
				}

				if(checkCompute != 0)
					x1 = compute(x1, d);
			}
			printf("%.10f\t%.10f\t%.10f%+.10fi\t%d\n", creal(x0[r][c]), cimag(x0[r][c]), creal(x1), cimag(x1), iterations);
			//x0[r][c] = color
		}
		r = r++;
		c = 0;
	}
}
	
int main() {

    struct timespec start;
    struct timespec end;
    struct timespec diff;

    timespec_get(&start, TIME_UTC);

    double d = 2.0;
	
	converge(d);
    
    timespec_get(&end, TIME_UTC);
    timespec_diff(&start, &end, &diff);

    printf("Time 1 :\t%lis and %fms\n", diff.tv_sec, (diff.tv_nsec / 1000000.0));

    return 0;
}