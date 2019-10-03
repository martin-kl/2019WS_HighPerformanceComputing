#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <omp.h>

#define FILENAME "cells"

void parseArguments(int argc, char *argv[], char *progname, short unsigned *threads);
long convertToInt(char *arg);

//--    main()              //////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    char *progname;
    short unsigned threads;

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

