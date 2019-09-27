#include <stdio.h>
#include <complex.h>

double x1r, x1c;
//complex c1,c2;
double fcn(double val, size_t pow) {
	if(pow == 1)
		return val;
	else if(pow == 0)
		return 1;

	return val*fcn(val, pow-1);
	}

void newton(size_t c, size_t d, double x0r, double x0c) {
	for(size_t i = 0; i < c; i++) {
	  double 
	  //x1r = x0r - ( (fcn(x0r, d) - 1) / (d*(fcn(x0r, (d-1)))) );
	  //		x1c = x0c - ( (fcn(x0c, d) - 1) / (d*(fcn(x0c, (d-1)))) );
		x0r = x1r;
		x0c = x1c;
		printf("%.14f\t%.14fi\n", x1r, x1c);
	}
}

  //z1r = creal(z)
  //z1i = cimag(z);
int main() {
  double complex c2;
  double complex c1;
  double complex d = 3;
	double xr = -3.0;
	double xc = 1.0;
	c1 = -3.0 + 1.0*I;
	size_t cnt = 10;
	//printf("Real\t\tComplex\n");
	//printf("%.14f\t%.14f\n", xr, xr);
	//	newton(cnt, d, xr, xc);

	for(int i = 10; i > 0; i--) {
	  c2 = c1 - ((cpow(c1, d)-1)/(d*cpow(c1, d-1)));
	  printf("%.15f + i%.15f\n", creal(c2), cimag(c2));
	  c1 = c2;
	}
	
	return 0;
}
	
