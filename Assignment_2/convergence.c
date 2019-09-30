#include <stdio.h>
#include <complex.h>
#include <math.h>
#define M_PI 3.14159265358979323846
#define E 2.71828182845904523536

int main(){

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

	size_t d = 9;
	double complex x0 = -3 + 1*I;
	double complex x1;
	double epsilon = 0.001;
	short int root_id = 0;
	double n_root = 100000000000;

	short int conv = -1;
	int iterations = 0;
	while(conv == -1) {
		iterations += 1;
		x1 = x0 - (cpow(x0,d)-1)/(d*cpow(x0,d-1));
	 	printf("%.15f, %.15f\n", creal(x1),cimag(x1));

		for (size_t ix = 0; ix < d; ix++) {
			if (cabs(x1 - root[d-1][ix] ) <= epsilon) {
				printf("this point converges to root number %ld = %.15f + %.15f i\n",ix + 1,creal(root[d-1][ix]),cimag(root[d-1][ix]));
				conv = 1;
				break;
			}
			if(cabs(creal(x1)) >= n_root || cabs(cimag(x1)) >= n_root || cabs(x1) <= 0.001){
				printf("special case x1 >= 10000000000\n");
				conv = 1;
				break;
			}
		}

		x0 = x1;
	}
	printf("number of iterations %d\n",iterations);

		return 0;
}
