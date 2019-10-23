#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>

#define FILENAME "cells"

#define MAX_DISTANCES 3466         //number of max distances since values are between -10 and 10 (00,00 .... 34,65)
#define MAX_POINTS_PER_BLOCK 10000 // number of points allowed to load each time in order to not exceed 1Mb
#define CHAR_PER_POINT 24          // number of chars per line i.e. per point including \n
#define CHAR_PER_COORDINATE 8      // 1 sign, 2 before comma, 1 comma, 3 after comma, 1 space or \n

//--    Prototypes              ///////////////////////////////////////////////////

int read_and_parse_naive(int n_chars_to_read, short points[][3], FILE *fp);
void substring(char s[], int p, int l);

//--    main()              ///////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    FILE *fp;

    size_t number_of_characters, number_of_fixed_points, blocks;
    const int max_read_char = MAX_POINTS_PER_BLOCK * CHAR_PER_POINT;

    //store points in 3 dimensional array
    short int fixed_points[MAX_POINTS_PER_BLOCK][3];

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
        number_of_fixed_points = read_and_parse_naive(number_of_characters, fixed_points, fp);
        printf("Read %lu points in one (and only) block!\n", number_of_fixed_points);
        //distance_within_block(number_of_fixed_points, fixed_points, distances);
    }
    else
    {
        //printf("### ### #### #### #### #### #### ####\n");
        //printf("\t\tmultiple blocks - starting\n");

        //read fixed block and calculate distance within this block
        for (size_t block_i = 0; block_i < blocks; block_i++)
        {
            //jump to position in file
            //fseek(fp, block_i * max_read_char, SEEK_SET);
            number_of_fixed_points = read_and_parse_naive(max_read_char, fixed_points, fp);
            printf("Read %lu points in block #%lu!\n", number_of_fixed_points, block_i + 1);
            //distance_within_block(number_of_fixed_points, fixed_points, distances);
        }
    }
    fclose(fp);
    exit(EXIT_SUCCESS);
}

//--    methods              //////////////////////////////////////////////////

int read_and_parse_naive(int n_chars_to_read, short points[][3], FILE *fp)
{
    float fpoints[3];
    size_t i = 0;
    while(n_chars_to_read >= CHAR_PER_POINT)
    {
        //size_t k = i * CHAR_PER_POINT + j * CHAR_PER_COORDINATE;
        if (fscanf(fp, "%f %f %f\n", &fpoints[0], &fpoints[1], &fpoints[2]) == EOF)
        {
            //printf("\t\tbreaking now\n");
            break;
        }
        n_chars_to_read -= CHAR_PER_POINT;
        //printf("%f, %f, %f - ", fpoints[0], fpoints[1], fpoints[2]);
        points[i][0] = (short)(fpoints[0] * 1000);
        points[i][1] = (short)(fpoints[1] * 1000);
        points[i][2] = (short)(fpoints[2] * 1000);
        //printf("reading number %lu: (%d, %d, %d)\n", i, points[i][0], points[i][1], points[i][2]);
        i++;
    }
    return i;
}
