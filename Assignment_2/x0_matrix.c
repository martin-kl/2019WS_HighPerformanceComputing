#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <complex.h>
#define MAX_LINES 10000// the maximum number of lines the program will accept, gives an integer overflow warning when using 100,000

int main() {
  double complex *x_0entries = (double complex*) malloc(MAX_LINES*MAX_LINES*sizeof(double complex));
  double complex **x_0 = (double complex**) malloc(MAX_LINES*sizeof(double complex*));
  int l = 10; // the parsed argument for the lines
  double complex div = (double complex) l - 1;

  for ( size_t ix = 0, jx = 0; ix < MAX_LINES; ++ix, jx+=MAX_LINES )
    x_0[ix] = x_0entries + jx;

  for ( size_t ix = 0; ix < l; ++ix ){
    for ( size_t jx = 0; jx < l; ++jx ){
      x_0[ix][jx] = -2 + 2*I + (double complex)jx*4/div - (double complex)ix*4*I/div;
      printf("(%0.3f,%0.3fi) ",creal(x_0[ix][jx]),cimag(x_0[ix][jx]));
      }
      printf("\n");
  }

  free(x_0);
  free(x_0entries);
  return 0;
}
