#include <stdio.h>
#include <complex.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LINES 10000// the maximum number of lines the program will accept, gives an integer overflow warning when using 100,000
#define M_PI 3.14159265358979323846


//Global variables
size_t d = 3; // the parsed argument for the power of the polynomial
int l = 2; // the parsed argument for the lines

//function declaration

void compute(double complex x0, short int *id, int *iterations);

int main() {

	//creation of initial values according to parsed parameters
	double complex *x_0entries = (double complex*) malloc(MAX_LINES*MAX_LINES*sizeof(double complex));
  double complex **x_0 = (double complex**) malloc(MAX_LINES*sizeof(double complex*));
  double complex div = (double complex) l - 1;

  for ( size_t ix = 0, jx = 0; ix < MAX_LINES; ++ix, jx+=MAX_LINES )
    x_0[ix] = x_0entries + jx;

  for ( size_t ix = 0; ix < l; ++ix ){
    for ( size_t jx = 0; jx < l; ++jx ){
      x_0[ix][jx] = -2 + 2*I + (double complex)jx*4/div - (double complex)ix*4*I/div;
      double complex x0 = x_0[ix][jx];
      //printf("(%0.3f,%0.3fi) ",creal(x0),cimag(x0));
      }
      //printf("\n");
  }
	short int root_id[l];
	int num_iter[l];
	for (size_t kx = 0; kx < l; kx++) {
		root_id[kx]=0;
		num_iter[kx]=0;
		double complex initial_value = x_0[0][kx];
		compute(initial_value, root_id + kx, num_iter + kx);
	}
	for (size_t i = 0; i < l; i++) {
		printf("initial value %f + %fi root %d iterations %d\n",creal(x_0[0][i]),cimag(x_0[0][i]),root_id[i], num_iter[i]);
			}


	free(x_0);
	free(x_0entries);
	return 0;
}

void compute(double complex x0, short int *id, int *iter){

	// hardcode roots
	double complex root[9][9] = {{1,0,0,0,0,0,0,0,0}, // roots for x - 1, d = 1
	                            {1,-1,0,0,0,0,0,0,0}, // roots for x^2 - 1, d = 2
															{1,-0.5 - (0.8660254037)*I,-0.5 + 0.8660254037*I,0,0,0,0,0,0}, // roots for x^3 - 1, d = 3
															{1,-1,I,-I,0,0,0,0,0}, // roots for x^4 - 1, d = 4
															{1,-0.8090169943 - 0.5877852522*I,-0.8090169943 + 0.5877852522*I,0.3090169943 - 0.9510565162*I,0.3090169943 + 0.9510565162*I,0,0,0,0}, // roots for x^5 - 1, d = 5
															{1,-1,-0.5 + 0.8660254037*I,0.5 - 0.8660254037*I,-0.5 - 0.8660254037*I,0.5 + 0.8660254037*I,0,0,0}, // roots for x^6 - 1, d = 6
															{1,-0.9009688679 - 0.4338837391*I,0.6234898018 + 0.7818314824*I,-0.2225209339 - 0.9749279121*I,-0.2225209339 + 0.9749279121*I,0.6234898018 - 0.7818314824*I,-0.9009688679 + 0.4338837391*I,0,0}, // roots for x^7 - 1, d = 7
															{1,-1,I,-I,0.7071067811 + 0.7071067811*I,-0.7071067811 - 0.7071067811*I,0.7071067811 - 0.7071067811*I,-0.7071067811 + 0.7071067811*I,0}, // roots for x^8 - 1, d = 8
															{1,-0.9396926207 - 0.3420201433*I,0.7660444431 + 0.6427876096*I,-0.5 - 0.8660254037*I,0.1736481776 + 0.9848077530*I,0.1736481776 - 0.9848077530*I,-0.5 + 0.8660254037*I,0.7660444431 - 0.6427876096*I,-0.9396926207 + 0.3420201433*I}}; // roots for x^9 - 1, d = 9



	double complex x1;
	double complex difference;
	double epsilon = 0.001;
	long int n_root = 100000000000;

	short int conv = -1;
	short int iterations;
	while(conv == -1) {
		iterations += 1;
		x1 = x0 - (cpow(x0,d)-1)/(d*cpow(x0,d-1));
	 	//printf("%.15f, %.15f\n", creal(x1),cimag(x1));

		for (size_t ix = 0; ix < d; ix++) {

			difference = x1 - root[d-1][ix];
			if (creal(difference)*creal(difference) + cimag(difference)*cimag(difference) <= epsilon*epsilon) {// trying not to use cabs()
				//printf("this point converges to root number %ld = %.15f + %.15f i\n",ix + 1,creal(root[d-1][ix]),cimag(root[d-1][ix]));
				*id = ix;
				conv = 1;
				break;
			}
			if(creal(x1) >= n_root || creal(x1) <= -n_root || cimag(x1) >= n_root || cimag(x1) <= -n_root || (creal(x1)*creal(x1) + cimag(x1)*cimag(x1)) <= epsilon*epsilon){ //trying not to use cabs()
				//printf("special case x1 >= 10000000000\n");
				*id = ix;
				conv = 1;
				break;
			}
		}

		x0 = x1;
	}
	*iter = iterations;
	//printf("number of iterations %d\n",iterations);


}
