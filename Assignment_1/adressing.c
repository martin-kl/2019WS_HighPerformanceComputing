#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "time_diff.h"

#define SIZE 1000000 //1 million
#define CALLS 1000

void inc1(int *x, int *y, int *p, int a) {
    int jx;
    for (size_t kx = 0; kx < SIZE; ++kx) {
        jx = p[kx];
        y[jx] += a * x[jx];
    }
}

void initialize(int *x, int*y) {
    //initialize x and y:
    int down = SIZE + 10;
    for(size_t i = 0; i < SIZE; i++) {
        x[i] = i;
        y[i] = down--;
    }
}


int main() {
    int * x;
    int * y;
    int * p; // index vector also with same length

    x = (int*) malloc(sizeof(int) * SIZE);
    y = (int*) malloc(sizeof(int) * SIZE);
    p = (int*) malloc(sizeof(int) * SIZE);

    struct timespec start;
    struct timespec end;
    struct timespec diff1;
    struct timespec diff2;
    struct timespec diff3;

    initialize(x, y);

    //first version:
    int m = 100; // 0; // 1.000 * 1.000 = 1.000.000 - that's this value
    int ix = 0;
    for (size_t jx = 0; jx < m; ++jx) {
        for (size_t kx = 0; kx < m; ++kx)
            p[jx + m*kx] = ix++;
    }

    printf("starting to call first version...\n");
    timespec_get(&start, TIME_UTC);
    for(size_t i = 0; i < CALLS; i++) {
        if(i % 2 == 0) 
            inc1(x, y, p, 3);
        else
            inc1(x, y, p, -2);
    }
    timespec_get(&end, TIME_UTC);
    timespec_diff(&start, &end, &diff1);
    
    printf("First version completed.\n");

    //reset values
    initialize(x, y);

    //second version:
    for (ix = 0; ix < SIZE; ++ix)
        p[ix] = ix;

    printf("starting to call second version...\n");
    timespec_get(&start, TIME_UTC);
    for(size_t i = 0; i < CALLS; i++) {
        if(i % 2 == 0) 
            inc1(x, y, p, 3);
        else
            inc1(x, y, p, -2);
    }
    timespec_get(&end, TIME_UTC);
    timespec_diff(&start, &end, &diff2);
 
    printf("Second version completed.\n");

    //reset values
    initialize(x, y);


    //TODO direct access implementation
    

    printf("Time needed for 1st impl.: %lis and %fms <- 'random' addressing\n", diff1.tv_sec, (diff1.tv_nsec / 1000000.0));
    printf("Time needed for 2nd impl.: %lis and %fms <- seq. addressing\n", diff2.tv_sec, (diff2.tv_nsec / 1000000.0));
}
