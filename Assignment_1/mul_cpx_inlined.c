#include <stdio.h>

#define LENGTH 30000

int main() {
    //call mul_cpx
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
    //since the calculations only lasts for some ms we have to do this more often
    for(int j = 0; j < 10000; j++) {
        for(int i = 0; i < LENGTH; i++) {
            asr[i] = (bsr[i] * csr[i]) - (bsi[i] * csi[i]);
            asi[i] = (bsr[i] * csi[i]) + (bsi[i] * csr[i]);
        }
    }

    printf("Inlined-calculation done.\n\n");

    printf("as[0]: %.2f + %.2fi\n", asr[0], asi[0]);
    printf("as[%d]: %.2f + %.2fi\n", LENGTH-1, asr[LENGTH-1], asi[LENGTH-1]);
}
