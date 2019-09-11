#include <stdio.h>
#include <time.h>

void timespec_diff(struct timespec *start, struct timespec *stop,
                   struct timespec *result) {
    if ((stop->tv_nsec - start->tv_nsec) < 0) {
        result->tv_sec = stop->tv_sec - start->tv_sec - 1;
        result->tv_nsec = stop->tv_nsec - start->tv_nsec + 1000000000;
    } else {
        result->tv_sec = stop->tv_sec - start->tv_sec;
        result->tv_nsec = stop->tv_nsec - start->tv_nsec;
    }

    return;
}

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
