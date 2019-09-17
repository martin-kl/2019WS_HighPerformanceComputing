#include <stdio.h>
#include <time.h>
#include "time_diff.h"

int main() {
    unsigned long sum;
    struct timespec start;
    struct timespec end;

    while(1) {
        sum = 0;
        timespec_get(&start, TIME_UTC);

        for(int i = 0; i < 1000000000; i++) {
            sum += i;
        }
        printf("Sum: %lu\n", sum);

        timespec_get(&end, TIME_UTC);
        struct timespec diff;
        timespec_diff(&start, &end, &diff);
        printf("Time needed: %lis and %lins\n", diff.tv_sec, diff.tv_nsec);
    }
}
