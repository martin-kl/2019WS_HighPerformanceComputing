#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <time.h>
#include "time_diff.h"

#define FILEHDD "hdd_testfile"
#define FILESSD "/run/mount/scratch/hpcuser046/ssd_testfile"

void read(struct timespec *start, struct timespec *end, struct timespec *diff, FILE *fp, const int max) {
    int number;
    timespec_get(start, TIME_UTC);
    while(fscanf(fp, "%d", &number) == 1) {
    }
    timespec_get(end, TIME_UTC);
    timespec_diff(start, end, diff);
    //number+1 since we start by writing 0
    assert((number+1) == max);
    return;
}

void write(struct timespec *start, struct timespec *end, struct timespec *diff, FILE *fp, const int max) {
    timespec_get(start, TIME_UTC);
    for(int i = 0; i < max; i++) {
        if(fprintf(fp, "%d ", i) <= 0) {
            fprintf(stderr, "Error writing to file...\n");
        }
    }
    timespec_get(end, TIME_UTC);
    timespec_diff(start, end, diff);
    return;
}



int main() {
    const int max = pow(2, 20);
    FILE *fp;

    struct timespec start;
    struct timespec end;

    struct timespec diffhddw;
    struct timespec diffhddr;

    struct timespec diffssdw;
    struct timespec diffssdr;

    printf("Starting to write to HDD...\n");

    if( (fp=fopen(FILEHDD, "w+")) == NULL) {
        fprintf(stderr, "Cannot open HDD file to write & read! Exiting.\n");
        exit(1);
    }

    write(&start, &end, &diffhddw, fp, max);

    // reading:
    // first jump to beginning of file:
    rewind(fp);
    printf("Starting to read from HDD...\n");
    read(&start, &end, &diffhddr, fp, max);
    fclose(fp);
    printf("Finished writing & reading HDD\n\n");





    //SSD:
    printf("Starting to write to SSD...\n");

    if( (fp=fopen(FILESSD, "w+")) == NULL) {
        fprintf(stderr, "Cannot open SSD file to write & read! Exiting.\n");
        exit(1);
    }

    write(&start, &end, &diffssdw, fp, max);

    // reading:
    // first jump to beginning of file:
    rewind(fp);
    printf("Starting to read from SSD...\n");
    read(&start, &end, &diffssdr, fp, max);
    fclose(fp);
    printf("Finished writing & reading SSD\n\n");


    printf("Time needed for hdd w.: %lis and %fms\n", diffhddw.tv_sec, (diffhddw.tv_nsec / 1000000.0));
    printf("Time needed for hdd r.: %lis and %fms\n", diffhddr.tv_sec, (diffhddr.tv_nsec / 1000000.0));
    printf("Time needed for ssd w.: %lis and %fms\n", diffssdw.tv_sec, (diffssdw.tv_nsec / 1000000.0));
    printf("Time needed for ssd r.: %lis and %fms\n", diffssdr.tv_sec, (diffssdr.tv_nsec / 1000000.0));

}
