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

/* Tries to read from the given file pointer n_chars_to_read characters and parses the coordinates.
 * If less characters are read, the method prints an error to stderr.
 * In either case, the really read number of points is returned.
 * The parsed coordinates are stored in points.
 */
int read_and_parse_sequential(int n_chars_to_read, short points[][3], FILE *fp);

//--    main()              ///////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    FILE *fp;

    size_t number_of_characters, number_of_fixed_points, blocks;
    const int max_read_char = MAX_POINTS_PER_BLOCK * CHAR_PER_POINT;

    //has to be an int, short is too small
    //int distances[MAX_DISTANCES] = { 0 };
    //store points as short, 3 * since we store the coordinates for a point after each other
    //i.e.: x1 y1 z1 x2 y2 z2 ...
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
        number_of_fixed_points = read_and_parse_sequential(number_of_characters, fixed_points, fp);
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
            fseek(fp, block_i * max_read_char, SEEK_SET);
            number_of_fixed_points = read_and_parse_sequential(max_read_char, fixed_points, fp);
            printf("Read %lu points in block #%lu!\n", number_of_fixed_points, block_i + 1);
            //distance_within_block(number_of_fixed_points, fixed_points, distances);
        }
    }
    fclose(fp);
    exit(EXIT_SUCCESS);
}

//--    methods              //////////////////////////////////////////////////

int read_and_parse_sequential(int n_chars_to_read, short points[][3], FILE *fp)
{
    char buffer[MAX_POINTS_PER_BLOCK * CHAR_PER_POINT];

    int chars_read = fread(buffer, sizeof(char), n_chars_to_read, fp);
    if (chars_read != n_chars_to_read)
    {
        fprintf(stderr, "Attention: wanted to read %d chars but read %d!\n", n_chars_to_read, chars_read);
    }
    int points_read = chars_read / CHAR_PER_POINT;
    //printf("read %d points, now starting to parse\n", points_read);

    //we can safely use collapse here since we have no dependencies
    for (size_t i = 0; i < points_read; i++)
    {
        size_t k1 = i * CHAR_PER_POINT + 0 * CHAR_PER_COORDINATE;
        size_t k2 = i * CHAR_PER_POINT + 1 * CHAR_PER_COORDINATE;
        size_t k3 = i * CHAR_PER_POINT + 2 * CHAR_PER_COORDINATE;
        points[i][0] = (buffer[k1 + 1] - '0') * 10000 +
                       (buffer[k1 + 2] - '0') * 1000 +
                       (buffer[k1 + 4] - '0') * 100 +
                       (buffer[k1 + 5] - '0') * 10 +
                       (buffer[k1 + 6] - '0');
        points[i][1] = (buffer[k2 + 1] - '0') * 10000 +
                           (buffer[k2 + 2] - '0') * 1000 +
                           (buffer[k2 + 4] - '0') * 100 +
                           (buffer[k2 + 5] - '0') * 10 +
                           (buffer[k2 + 6] - '0');
        points[i][2] = (buffer[k3 + 1] - '0') * 10000 +
                           (buffer[k3 + 2] - '0') * 1000 +
                           (buffer[k3 + 4] - '0') * 100 +
                           (buffer[k3 + 5] - '0') * 10 +
                           (buffer[k3 + 6] - '0');
        if (buffer[k1] == '-')
            points[i][0] *= -1;
        if (buffer[k2] == '-')
            points[i][1] *= -1;
        if (buffer[k3] == '-')
            points[i][2] *= -1;
    }
    return points_read;
}
