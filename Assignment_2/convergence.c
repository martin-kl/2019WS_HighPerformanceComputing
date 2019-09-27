#include <stdio.h>
#include <complex.h>
#include <math.h>
int main() {


	size_t poly = 3;
	double complex x0 = -3 + 1*I;
	double complex x1;
	double epsilon = 0.001;
	double complex root[4] = {1,-0.5 + 0.5*sqrt(3)*I,-0.5 - 0.5*sqrt(3)*I};
	short int root_id = 0;
	double n_root = 100000000000;

	short int conv = -1;
	int iterations = 0;
	while(conv == -1) {
		iterations += 1;
		x1 = x0 - (cpow(x0,poly)-1)/(poly*cpow(x0,poly-1));
	 	printf("%.15f, %.15f\n", creal(x1),cimag(x1));

		for (size_t ix = 0; ix < poly + 1; ix++) {
			if (cabs(x1 - root[ix] ) <= epsilon) {
				printf("this point converges to root number %ld = %.15f + %.15f\n",ix,creal(root[ix]),cimag(root[ix]));
				conv = 1;
				break;
			}
		}

		x0 = x1;
	}
	printf("%d\n",iterations);

		return 0;
}
