#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 1000

// Given implementation of row and column sumns
void row_sums(double * sums, const double ** matrix, size_t nrs, size_t ncs) {
  for ( size_t ix=0; ix < nrs; ++ix ) {
    double sum = 0;
    for ( size_t jx=0; jx < ncs; ++jx )
      sum += matrix[ix][jx];
    sums[ix] = sum;
  }
}

void col_sums(double * sums, const double ** matrix, size_t nrs, size_t ncs) {
  for ( size_t jx=0; jx < ncs; ++jx ) {
    double sum = 0;
    for ( size_t ix=0; ix < nrs; ++ix )
      sum += matrix[ix][jx];
    sums[jx] = sum;
  }
}

void timespec_diff(struct timespec *start, struct timespec *stop,
                   struct timespec *result) {
    if ((stop->tv_nsec - start->tv_nsec) < 0) {
        result->tv_sec = stop->tv_sec - start->tv_sec - 1;
        result->tv_nsec = stop->tv_nsec - start->tv_nsec + 1000000000;
    } else {
        result->tv_sec = stop->tv_sec - start->tv_sec;
        result->tv_nsec = stop->tv_nsec - start->tv_nsec;
    }

    return;
}



// #### #### #### #### #### #### #### #### #### #### #### #### #### #### #### 

int main() {
    struct timespec start;
    struct timespec end;
    struct timespec diffrows;
    struct timespec diffcols;
    double * sumrows = (double*) malloc(sizeof(double) * SIZE);
    double * sumcols = (double*) malloc(sizeof(double) * SIZE);

    // Avoiding memory fragmentation with row-major order:
    int counter = 1;
    double * startaddr = (double*) malloc(sizeof(double) * SIZE*SIZE);

    double ** array = (double**) malloc(sizeof(double*) * SIZE);
    //build pointers to different rows:
    for ( size_t ix = 0, jx = 0; ix < SIZE; ++ix, jx+=SIZE ) {
      array[ix] = startaddr + jx;
    }
    /*
    // or like this with const:
    const double ** array = (const double**) malloc(sizeof(double*) * SIZE);
    // but then the for loop has to go since we have constants
    */

    for ( size_t ix = 0; ix < SIZE; ++ix ) {
      for ( size_t jx = 0; jx < SIZE; ++jx ) {
        array[ix][jx] = counter;
      }
    }

// #### #### #### #### #### #### #### #### #### #### #### #### #### #### #### 

    printf("Starting calculation and benchmarking...\n\n");

    timespec_get(&start, TIME_UTC);
    row_sums(sumrows, array, SIZE, SIZE);
    timespec_get(&end, TIME_UTC);
    timespec_diff(&start, &end, &diffrows);

    timespec_get(&start, TIME_UTC);
    col_sums(sumcols, array, SIZE, SIZE);
    timespec_get(&end, TIME_UTC);
    timespec_diff(&start, &end, &diffcols);

    printf("Row[0]-Sum: %f, time needed to calculate all row-sums: %lis and %lins\n", sumrows[0], diffrows.tv_sec, diffrows.tv_nsec);
    printf("Col[0]-Sum: %f, time needed to calculate all col-sums: %lis and %lins\n", sumcols[0], diffcols.tv_sec, diffcols.tv_nsec);
}
