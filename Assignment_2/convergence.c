#include <stdio.h>
#include <complex.h>
#include <math.h>
int main() {


	size_t d = 3;
	double complex x0 = -3 + 1*I;
	double complex x1;
	double epsilon = 0.001;
	double complex root[3] = {1,-0.5 + 0.5*sqrt(3)*I,-0.5 - 0.5*sqrt(3)*I};
	short int root_id = 0;

for (size_t i = 0; i < 10; i++) {
	x1 = x0 - (cpow(x0,d)-1)/(d*cpow(x0,d-1));
 	printf("%.15f, %.15f\n", creal(x1),cimag(x1));
	if (cabs(x1 - root[0] ) <= epsilon) {
		root_id = 0;
		printf("this point converges to root number %d = %.15f + %.15f\n",root_id,creal(root[root_id]),cimag(root[root_id]));
		break;
	}
	else if (cabs(x1 - root[1] ) <= epsilon) {
		root_id = 1;
		printf("this point converges to root number %d = %.15f + %.15f\n",root_id,creal(root[root_id]),cimag(root[root_id]));
		break;
	}
	else if (cabs(x1 - root[2] ) <= epsilon) {
		root_id = 2;
		printf("this point converges to root number %d = %.15f + %.15f\n",root_id,creal(root[root_id]),cimag(root[root_id]));
		break;
	}
	x0 = x1;
}

	return 0;
}
