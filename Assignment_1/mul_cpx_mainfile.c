#include <stdio.h>

#define LENGTH 30000

void mul_cpx( double * a_re, double * a_im, double * b_re, double * b_im, double * c_re, double * c_im) {
    *a_re = (*b_re * *c_re) - (*b_im * *c_im);
    *a_im = (*b_re * *c_im) + (*b_im * *c_re);
}

int main() {
    double asr[LENGTH];
    double asi[LENGTH];
    double bsr[LENGTH];
    double bsi[LENGTH];
    double csr[LENGTH];
    double csi[LENGTH];
    
    //populate b and c
    double down = LENGTH + 10.0;
    double up = 1.0;
    for(int i = 0; i < LENGTH; i++) {
        bsr[i] = up + 1.0;
        bsi[i] = up;
        csr[i] = down - 1.0;
        csi[i] = down;
        down -= 0.5;
        up += 0.5;
    }


    //start calculation:
    for(int i = 0; i < LENGTH; i++) {
        mul_cpx(&asr[i], &asi[i], &bsr[i], &bsi[i], &csr[i], &csi[i]);
        // printf("as[%d]: %.2f + %.2fi\n", i, asr[i], asi[i]);
    }

    printf("Mainfile-calculation done.\n\n");

    printf("as[0]: %.2f + %.2fi\n", asr[0], asi[0]);
    printf("as[%d]: %.2f + %.2fi\n", LENGTH-1, asr[LENGTH-1], asi[LENGTH-1]);
}
