#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <omp.h>
#include <math.h>

#define FILENAME "cells"

#define MAX_DISTANCES 3466         //number of max distances since values are between -10 and 10 (00,00 .... 34,65)
#define MAX_POINTS_PER_BLOCK 10000 // number of points allowed to load each time in order to not exceed 1Mb
#define CHAR_PER_POINT 24          // number of chars per line i.e. per point including \n
#define CHAR_PER_COORDINATE 8      // 1 sign, 2 before comma, 1 comma, 3 after comma, 1 space or \n

//--    Prototypes              ///////////////////////////////////////////////////

void parseArguments(int argc, char *argv[], char *progname, short unsigned *threads);

long convertToInt(char *arg);

int read_and_parse_parallel(int number_of_characters, short *fixed_points, FILE *fp);

static inline void distance_within_block(size_t number_of_points, short *points, int *distances);

//--    main()              ///////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    char *progname;
    FILE *fp;
    short unsigned threads;

    size_t number_of_characters, number_of_fixed_points, blocks;
    const int max_read_char = MAX_POINTS_PER_BLOCK * CHAR_PER_POINT;

    //has to be an int, short is too small
    int distances[MAX_DISTANCES] = { 0 };
    //store points as short, 3 * since we store the coordinates for a point after each other
    //i.e.: x1 y1 z1 x2 y2 z2 ...
    short int fixed_points[3 * MAX_POINTS_PER_BLOCK];

    //get program name
    if (argc > 0)
        progname = argv[0];
    else
    {
        fprintf(stderr, "Error: no program name can be found\n");
        exit(EXIT_FAILURE);
    }

    parseArguments(argc, argv, progname, &threads);
    //set max number of threads for omp
    omp_set_num_threads(threads);

    if ((fp = fopen(FILENAME, "r")) == NULL)
    {
        fprintf(stderr, "Cannot open \"%s\" file to read! Exiting.\n", FILENAME);
        exit(EXIT_FAILURE);
    }

    //get number of points in file
    //move to file end with offset 0
    fseek(fp, 0, SEEK_END);
    //ftell gives us the current position in the file which is equal to the number of bytes if called at end of file
    number_of_characters = ftell(fp);
    blocks = number_of_characters / max_read_char;
    //check if we have a not full part
    if (number_of_characters % max_read_char != 0)
    {
        blocks++;
    }
    //move back to beginning
    fseek(fp, 0, SEEK_SET);

    //if there is only one block we can simply load everything at once:
    if (blocks == 1)
    {
        //printf("### ### #### #### #### #### #### ####\n");
        //printf("\t\tone block - starting\n");
        number_of_fixed_points = read_and_parse_parallel(number_of_characters, fixed_points, fp);
        distance_within_block(number_of_fixed_points, fixed_points, distances);
    }
    else
    {
        //printf("### ### #### #### #### #### #### ####\n");
        //printf("\t\tmultiple blocks - starting\n");
        //read fixed block and calculate distance within this block
        for (size_t block_i = 0; block_i < blocks; block_i++)
        {
            //jump to position in file
            fseek(fp, block_i * max_read_char, SEEK_SET);
            number_of_fixed_points = read_and_parse_parallel(max_read_char, fixed_points, fp);
            distance_within_block(number_of_fixed_points, fixed_points, distances);

            //now read other block(s) and calculate distances between
            for (size_t block_j = block_i + 1; block_j < blocks; block_j++)
            {
                int chars_in_block = max_read_char;
                size_t points_in_block = MAX_POINTS_PER_BLOCK;
                int points_read;

                if (block_j == blocks - 1 && (number_of_characters % max_read_char != 0))
                {
                    //last block and block net full
                    chars_in_block = number_of_characters % max_read_char;
                    points_in_block = chars_in_block % CHAR_PER_POINT;
                }

		//we only need 3 * the number of points in a block to store the coordinates
                short block_points[3 * points_in_block];
                points_read = read_and_parse_parallel(chars_in_block, block_points, fp);

                //calculate distance
#pragma omp parallel for reduction(+:distances[:MAX_DISTANCES])
                for (size_t i = 0; i < number_of_fixed_points; ++i)
                {
                    for (size_t j = 0; j < points_read; ++j)
                    {
                        double dx = (fixed_points[3 * i] - block_points[3 * j]);
                        double dy = (fixed_points[3 * i + 1] - block_points[3 * j + 1]);
                        double dz = (fixed_points[3 * i + 2] - block_points[3 * j + 2]);
                        short idx = (short)(sqrt(dx * dx + dy * dy + dz * dz) / 10.0);
                        ++distances[idx];
                    }
                }
            }
        }
    }
    fclose(fp);

    //printf("finished, now printing values:\n");
    for (size_t i = 0; i < MAX_DISTANCES; i++)
        if (distances[i] != 0)
            printf("%02lu.%02lu %i\n", i / 100, i % 100, distances[i]);

    exit(EXIT_SUCCESS);
}

//--    methods              //////////////////////////////////////////////////

int read_and_parse_parallel(int n_chars_to_read, short *points, FILE *fp)
{
    char buffer[MAX_POINTS_PER_BLOCK * CHAR_PER_POINT];

    int chars_read = fread(buffer, sizeof(char), n_chars_to_read, fp);
    if(chars_read != n_chars_to_read) {
	fprintf(stderr, "Attention: wanted to read %d chars but read %d!\n", n_chars_to_read, chars_read);
    }
    int points_read = chars_read / CHAR_PER_POINT;
    //printf("read %d points, now starting to parse\n", points_read);

    //we can safely use collapse here since we have no dependencies
#pragma omp parallel for collapse(2)
    for (size_t i = 0; i < points_read; i++)
    {
        for (size_t j = 0; j < 3; j++)
        {
            size_t k = i * CHAR_PER_POINT + j * CHAR_PER_COORDINATE;
            points[3 * i + j] = (buffer[k + 1] - '0') * 10000 +
                                (buffer[k + 2] - '0') * 1000 +
                                (buffer[k + 4] - '0') * 100 +
                                (buffer[k + 5] - '0') * 10 +
                                (buffer[k + 6] - '0');
            if (buffer[k] == '-')
                points[3 * i + j] *= -1;
        }
    }
    return points_read;
}

static inline void distance_within_block(size_t number_of_points, short *points, int *distances)
{
#pragma omp parallel for reduction(+:distances[:MAX_DISTANCES])
    for (size_t i = 0; i < number_of_points; i++)
    {
        for (size_t j = i + 1; j < number_of_points; j++)
        {
            double dx = points[3 * i] - points[3 * j];
            double dy = points[3 * i + 1] - points[3 * j + 1];
            double dz = points[3 * i + 2] - points[3 * j + 2];
            short idx = (short)(sqrt(dx * dx + dy * dy + dz * dz) / 10.0);
            ++distances[idx];
        }
    }
}

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
