#include <stdio.h>
#include <time.h>
#include "time_diff.h"

#define LOOP 100

int main() {
    unsigned long sum;
    struct timespec start;
    struct timespec end;
    struct timespec diff;

    timespec_get(&start, TIME_UTC);
    for(int i = 0; i < LOOP; i++) {
        sum = 0;
        for(int i = 0; i < 1000000000; i++) {
            sum += i;
        }
    }
    printf("Last Sum: %lu\n\n", sum);
    timespec_get(&end, TIME_UTC);
    timespec_diff(&start, &end, &diff);
    printf("Overall time needed for %d iterations: %lis and %lins\n", LOOP, diff.tv_sec, diff.tv_nsec);
    double avg = diff.tv_sec * 1000;
    avg += (diff.tv_nsec / 1000000.0);
    avg /= LOOP;
    printf("Avg time: %fms\n", avg);
}
