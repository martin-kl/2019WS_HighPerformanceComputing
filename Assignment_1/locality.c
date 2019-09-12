#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "time_diff.h"

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

// Improved column sums implementation
void col_sums2(double * sums, const double ** matrix, size_t nrs, size_t ncs) {
    for (size_t i = 0; i < nrs; i++) {
        for (size_t j = 0; j < ncs; j++) {
            sums[j] += matrix[i][j];
        }
    }
}


// #### #### #### #### #### #### #### #### #### #### #### #### #### #### #### 

int main() {
    struct timespec start;
    struct timespec end;
    struct timespec diffrows;
    struct timespec diffcols;
    double * sumrows = (double*) malloc(sizeof(double) * SIZE);
    double * sumcols = (double*) malloc(sizeof(double) * SIZE);
    //structs for improved col sums
    struct timespec diffcols2;
    double * sumcols2 = (double*) malloc(sizeof(double) * SIZE);

    // Avoiding memory fragmentation with row-major order:
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

    //populate array
    double counter = 0.25;
    for ( size_t ix = 0; ix < SIZE; ++ix ) {
      for ( size_t jx = 0; jx < SIZE; ++jx ) {
        array[ix][jx] = counter;
        counter += 0.25;
      }
    }

// #### #### #### #### #### #### #### #### #### #### #### #### #### #### #### 

    printf("Starting calculation and benchmarking...\n");

    timespec_get(&start, TIME_UTC);
    row_sums(sumrows, array, SIZE, SIZE);
    timespec_get(&end, TIME_UTC);
    timespec_diff(&start, &end, &diffrows);

    timespec_get(&start, TIME_UTC);
    col_sums(sumcols, array, SIZE, SIZE);
    timespec_get(&end, TIME_UTC);
    timespec_diff(&start, &end, &diffcols);

    timespec_get(&start, TIME_UTC);
    col_sums2(sumcols2, array, SIZE, SIZE);
    timespec_get(&end, TIME_UTC);
    timespec_diff(&start, &end, &diffcols2);

    //check for correctness of col_sums2
    printf("Calculations are done, checking for correctness...\n");
    for(size_t i = 0; i < SIZE; i++) {
        assert(sumcols[i] == sumcols2[i]);
    }
    printf("Results are correct!\n\n");

    printf("Row[0]-Sum: %f,\t time needed to calculate all row-sums: %lis and %fms\n", sumrows[0], diffrows.tv_sec, (diffrows.tv_nsec / 1000000.0));
    printf("Col[0]-Sum: %f,\t time needed to calculate all col-sums: %lis and %fms\n", sumcols[0], diffcols.tv_sec, (diffcols.tv_nsec / 1000000.0));
    printf("Col[0]-Sum: %f,\t time needed to calculate all col-sums: %lis and %fms <-- with improved algorithm\n", sumcols2[0], diffcols2.tv_sec, (diffcols2.tv_nsec / 1000000.0));
}
