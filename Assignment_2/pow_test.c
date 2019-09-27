#include <stdio.h>
#include <complex.h>
#include <math.h>
#include <time.h>

#define NUMBER 100000
#define M_PI 3.14159265358979323846

/*
compile with:
gcc -o pow_test pow_test.c -lm -O2
*/

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


static inline double complex customPow1(double complex x, double d)
{
    double complex res = x;
    for(size_t i = 0; i < d-1; i++)
    {
        res = res * x;
    }
    return res;
}


//second possible method inspired by:
// https://www.algebra.com/algebra/homework/complex/Raising-a-complex-number-to-an-integer-power.lesson
static inline double complex customPow2(double complex x, double d)
{
    double real = creal(x);
    double imag = cimag(x);

    //either calculate it ourselves:
    //given functions seem to perform as well as these calculations
    /*
    double add_for_correction = 0;
    if(real < 0) add_for_correction = M_PI;
    else if (imag < 0) add_for_correction = 2 * M_PI;
    double r = sqrt(real * real + imag *imag);
    double theta = atan(imag / real) + add_for_correction;
    */

    //or use given functions:
    double r = cabs(x);
    double theta = carg(x);

    //printf("\tr=%f\n", r);
    //printf("\ttheta=%f\n", theta);

    double complex res = pow(r, d) * (cos(d * theta) + I * sin(d * theta));
    return res;
}

int main()
{
    struct timespec start;
    struct timespec end;
    struct timespec diff1;
    struct timespec diff2;
    struct timespec diff3;

    double complex x = -1.8 + 1.2 * I;
    double d = 7.0;
    double complex res1;
    double complex res2;
    double complex res3;

    //cpow
    timespec_get(&start, TIME_UTC);
    for (size_t i = 0; i < NUMBER; ++i)
    {
        res1 = cpow(x, d);
    }
    timespec_get(&end, TIME_UTC);
    timespec_diff(&start, &end, &diff1);

    //custom implementation
    timespec_get(&start, TIME_UTC);
    for (size_t i = 0; i < NUMBER; ++i)
    {
        res2 = customPow1(x, d);
    }
    timespec_get(&end, TIME_UTC);
    timespec_diff(&start, &end, &diff2);

    //custom implementation
    timespec_get(&start, TIME_UTC);
    for (size_t i = 0; i < NUMBER; ++i)
    {
        res3 = customPow2(x, d);
    }
    timespec_get(&end, TIME_UTC);
    timespec_diff(&start, &end, &diff3);


    printf("Result cpow:\t%.10f%+.10fi\n", creal(res1), cimag(res1));
    printf("Result custom1:\t%.10f%+.10fi\n", creal(res2), cimag(res2));
    printf("Result custom2:\t%.10f%+.10fi\n", creal(res3), cimag(res3));
    printf("\n");
    printf("Time cpow.:\t%lis and %fms\n", diff1.tv_sec, (diff1.tv_nsec / 1000000.0));
    printf("Time custom1:\t%lis and %fms\n", diff2.tv_sec, (diff2.tv_nsec / 1000000.0));
    printf("Time custom2:\t%lis and %fms\n", diff3.tv_sec, (diff3.tv_nsec / 1000000.0));
}