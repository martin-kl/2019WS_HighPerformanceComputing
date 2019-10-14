#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <omp.h>

#define FILENAME "cells"
#define MAX_DIST_NUM 2828
#define ALLOWED_BLOCK_SIZE 10 // number of points allowed to load each time in order to not exceed 1Mb
#define FIXED_BLOCK_SIZE 5 // number of chars in once
#define NUM_CHAR 25 // number of chars per line i.e. per point

void parseArguments(int argc, char *argv[], char *progname, short unsigned *threads);
long convertToInt(char *arg);



//--    main()              ///////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    char *progname;
    short unsigned threads;
    char *allowed_block = malloc((ALLOWED_BLOCK_SIZE)*(NUM_CHAR*sizeof(char)));
    char *fixed_points = malloc((FIXED_BLOCK_SIZE)*(NUM_CHAR*sizeof(char)));

    FILE *fp;
    //TODO think about reading in points
    //double **points;

    //get program name
    if (argc > 0)
    {
        progname = argv[0];
    }
    else
    {
        fprintf(stderr, "Error: no program name can be found\n");
        exit(EXIT_FAILURE);
    }
    parseArguments(argc, argv, progname, &threads);
    printf("Called with # threads = %d\n", threads);

    if ( (fp = fopen(FILENAME, "r")) == NULL) {
        fprintf(stderr, "Cannot open cells file to read! Exiting.\n");
        exit(EXIT_FAILURE);
    }

    //set max number of threads for omp
    if(threads < omp_get_max_threads()) {
        omp_set_num_threads(threads);
    }




    //finding distances:
    // creation of vector with all the possible distances stored in order: min 0.01 max 28.28
    float p_dist[MAX_DIST_NUM][2];
    for (size_t ix = 0; ix < MAX_DIST_NUM; ix++) {
      p_dist[ix][0] += 0.01;
    }
    //store current distance
    float dist_temp;
    size_t read_block_items;
    size_t read_fixed_items;
    int count = 0;

    while ((read_fixed_items = fread(fixed_points, sizeof(char), NUM_CHAR*FIXED_BLOCK_SIZE, fp)) > 0) {
      printf("fixed_points\n");
      printf("%s ", fixed_points);
      count ++;

      while ((read_block_items = fread(allowed_block, sizeof(char), NUM_CHAR*ALLOWED_BLOCK_SIZE, fp)) > 0){
        printf("allowed_block\n");
        printf("%s ", allowed_block);
        //printf("\n");
        for (size_t ix = 0; ix < FIXED_BLOCK_SIZE; ix++) {


          for (size_t kx = 0; kx < (ALLOWED_BLOCK_SIZE - ix);kx++) {

            dist_temp = fixed_points[ix] - allowed_block[kx]; // calculating distances from each element of fixed block to all the elements of the current allowed block

            //counting specific distance
            for (size_t jx = 0; jx < MAX_DIST_NUM; jx++) {
              if (dist_temp == p_dist[jx][0])
                p_dist[jx][1] ++;
            }

          }
        }
      }
      fseek(fp,count*NUM_CHAR*FIXED_BLOCK_SIZE,SEEK_SET);
      //printf("\n");
    }

    free(allowed_block);
    free(fixed_points);
    fclose(fp);
    exit(EXIT_SUCCESS);
}

//--    methods              //////////////////////////////////////////////////

void parseArguments(int argc, char *argv[], char *progname, short unsigned *threads)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s -t<threads>\n", progname);
        exit(EXIT_FAILURE);
    }

    int opt;
    while ((opt = getopt(argc, argv, "t:")) != -1)
    {
        switch (opt)
        {
        case 't':
            *threads = convertToInt(optarg);
            break;
        default:
            fprintf(stderr, "Usage: %s -t<threads>\n", progname);
            exit(EXIT_FAILURE);
        }
    }
    //check for validity of arguments:
    if (*threads <= 0)
    {
        fprintf(stderr, "Error: Given number of threads is invalid/missing!\n");
        exit(EXIT_FAILURE);
    }
}

long convertToInt(char *arg)
{
    char *endptr;
    long number;

    //10 = decimal system, endptr is to check if strtol gave us a number
    number = strtol(arg, &endptr, 10);

    if ((errno == ERANGE && (number == LONG_MAX || number == LONG_MIN)) || (errno != 0 && number == 0))
    {
        fprintf(stderr, "Failed to convert input to number!\n");
        exit(EXIT_FAILURE);
    }
    if (endptr == arg)
    {
        fprintf(stderr, "No digits where found!\n");
        exit(EXIT_FAILURE);
    }

    /* If we got here, strtol() successfully parsed a number */
    if (*endptr != '\0')
    { /* In principle not necessarily an error... */
        fprintf(stderr, "Attention: further characters after number: %s\n", endptr);
        exit(EXIT_FAILURE);
    }
    return number;
}
