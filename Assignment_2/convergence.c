#include <stdio.h>
#include <complex.h>
#include <math.h>
#define M_PI 3.14159265358979323846

int main(){

	// hardcode roots
	double complex root[9][9] = {{1,0,0,0,0,0,0,0,0}, // roots for x - 1, d = 1
	                            {1,-1,0,0,0,0,0,0,0}, // roots for x^2 - 1, d = 2
															{1,-0.5 + 0.5*sqrt(3)*I,-0.5 - 0.5*sqrt(3)*I,0,0,0,0,0,0}, // roots for x^3 - 1, d = 3
															{1,-1,I,-I,0,0,0,0,0}, // roots for x^4 - 1, d = 4
															{1,-1/4 - sqrt(5)/4 - I*sqrt(5/8 - sqrt(5)/8),-1/4 - sqrt(5)/4 + I*sqrt(5/8 - sqrt(5)/8),-1/4 + sqrt(5)/4 - I*sqrt(5/8 + sqrt(5)/8),-1/4 + sqrt(5)/4 + I*sqrt(5/8 + sqrt(5)/8),0,0,0,0}, // roots for x^5 - 1, d = 5
															{1,-1,-0.5 - 0.5*sqrt(3)*I,0.5 + 0.5*sqrt(3)*I, 0.5 - 0.5*sqrt(3)*I,-0.5 + 0.5*sqrt(3)*I,0,0,0}, // roots for x^6 - 1, d = 6
															{1,-cos(M_PI/7) - sin(M_PI/7)*I,sin((3*M_PI)/14) + cos((3*M_PI)/14)*I,-sin(M_PI/14) - cos(M_PI/14)*I,-sin(M_PI/14) + cos(M_PI/14)*I,sin((3*M_PI)/14) - cos((3*M_PI)/14)*I,-cos(M_PI/7) + sin(M_PI/7)*I,0,0}, // roots for x^7 - 1, d = 7
															{1,-1,I,-I,(1 + I)/sqrt(2),(-1 - I)/sqrt(2),(1 - I)/sqrt(2),(-1 + I)/sqrt(2),0}, // roots for x^8 - 1, d = 8
															{1,-cos(M_PI/9) - I*sin(M_PI/9),0.5*sqrt(3)*sin(M_PI/9) + 0.5*cos(M_PI/9) + I*(0.5*sqrt(3)*cos(M_PI/9) - 0.5*sin(M_PI/9)),-0.5 - 0.5*I*sqrt(3),sin(M_PI/18) + I*cos(M_PI/18),sin(M_PI/18) - I*cos(M_PI/18),-0.5* + 0.5*I*sqrt(3),0.5*sqrt(3)*sin(M_PI/9) + 0.5*cos(M_PI/9) + I*(0.5*sin(M_PI/9) - 0.5*sqrt(3)*cos(M_PI/9)),-cos(M_PI/9) + I*sin(M_PI/9)}}; // roots for x^9 - 1, d = 9

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

		for (size_t ix = 0; ix < d + 1; ix++) {
			if (cabs(x1 - root[d-1][ix] ) <= epsilon) {
				printf("this point converges to root number %ld = %.15f + %.15f\n",ix,creal(root[d-1][ix]),cimag(root[d-1][ix]));
				conv = 1;
				break;
			}
		}

		x0 = x1;
	}
	printf("number of iterations %d\n",iterations);

		return 0;
}
