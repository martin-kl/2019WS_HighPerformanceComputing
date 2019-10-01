#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <time.h> // for nanosleep
#include <pthread.h>

//--    prototypes              //////////////////////////////////////////////////

/*
 */
long convertToInt(char *arg);
/*
 */
void parseArguments(int argc, char *argv[], char *progname, 
    short unsigned int *nmb_threads, unsigned int *nmb_lines, short unsigned int *poly);

/*
 */
void *compute_main(void *args);

/*
 */
void *write_method(void *args);

short unsigned int nmb_threads = 0; //number of threads
unsigned int nmb_lines = 0;         //nmb_lines can be 100.000 -> short int would be too small
short unsigned int poly = 0;

//TODO check if these data types are correct / perfect for us
// [Martin:] since we have not so many roots char should be enough ?!
short int **attractors;// roots
short int **convergences;// number iterations

char *item_done;

struct timespec sleep100ms = {0, 100000000L}; // 100 ms
pthread_mutex_t item_done_mutex;

//--    main()              //////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    char *progname;

    pthread_t *compute_threads;
    pthread_t write_thread;

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
    parseArguments(argc, argv, progname, &nmb_threads, &nmb_lines, &poly);
    printf("T is %d, L is %d and poly is %d\n", nmb_threads, nmb_lines, poly);

    //malloc memory for writing
    attractors = malloc(sizeof(short int *) * nmb_lines);
    convergences = malloc(sizeof(short int *) * nmb_lines);
    item_done = calloc(nmb_lines, sizeof(char)); //use calloc here so it is for sure 0

    //create nmb_threads compute threads and one writing thread
    compute_threads = (pthread_t *)malloc(sizeof(pthread_t) * nmb_threads);
    for (size_t tx = 0; tx < nmb_threads; ++tx)
    {
        size_t *args = malloc(sizeof(size_t));
        *args = tx;
        pthread_create(&compute_threads[tx], NULL, compute_main, (void *)args);
    }
    pthread_create(&write_thread, NULL, write_method, NULL);

    //joining threads again...
    int ret;
    for (size_t tx = 0; tx < nmb_threads; ++tx)
    {
        if ((ret = pthread_join(compute_threads[tx], NULL)))
        {
            fprintf(stderr, "Error joining one of the compute threads: %d\n", ret);
            exit(EXIT_FAILURE);
        }
    }
    if ((ret = pthread_join(write_thread, NULL)))
    {
        fprintf(stderr, "Error joining write thread: %d\n", ret);
        exit(EXIT_FAILURE);
    }
    pthread_mutex_destroy(&item_done_mutex);

    //free variables again
    free(attractors);
    free(convergences);
    free(item_done);
    free(compute_threads);

    return(EXIT_SUCCESS);
}

//--    Methods              //////////////////////////////////////////////////

void *compute_main(void *args)
{
    //offset is just the "number" of the thread in our case
    size_t offset = *((size_t *)args);
    free(args);

    for (size_t ix = offset; ix < nmb_lines; ix += nmb_threads)
    {
        //printf("\tThread %ld calculates number for row %ld\n", offset, ix);

        short int * attractor = (short int*)malloc(sizeof(short int) * nmb_lines);   //nmb_lines = nmb_rows
        short int * convergence = (short int*)malloc(sizeof(short int) * nmb_lines); //nmb_lines = nmb_rows

        //function to compute roots
        //arguments: hardcoded value of roots for the given poly
        //while the difference between the current step of newtons
        //method and any of the roots is greater than 10^-3
        //keep computing next iterations
        //store the number of iterations --> convergences
        //store or point to the value of the closest root when convergences
        //criteria is reached, or to the value for the exceptions
        //just for now to be able to write
        for (size_t cx = 0; cx < nmb_lines; ++cx)
        {
            attractor[cx] = 0;
            //convergence[cx] = 0;
            convergence[cx] = offset;
        }
        //TODO remove it later on - just for now sleep a little bit
        nanosleep(&sleep100ms, NULL);
        // compute work item

        attractors[ix] = attractor;
        convergences[ix] = convergence;

        pthread_mutex_lock(&item_done_mutex);
        item_done[ix] = 1;
        pthread_mutex_unlock(&item_done_mutex);
    }
    return NULL;
}

void *write_method(void *args)
{
    struct timespec sleep2ms = {0, 2000000L}; // 2 ms

    FILE *attr_file;
    FILE *conv_file;
    char attr_file_name[25]; //we need 25 chars since sprintf automatically null terminates!!
    sprintf(attr_file_name, "newton_attractors_x%hu.ppm", poly);
    char conv_file_name[26];
    sprintf(conv_file_name, "newton_convergence_x%hu.ppm", poly);

    short int *res_attr; //used to point to current row to handle
    short int *res_conv; //used to point to current row to handle

    if ((attr_file = fopen(attr_file_name, "w")) == NULL)
    {
        fprintf(stderr, "Cannot open attractor file to write! Exiting.\n");
        exit(EXIT_FAILURE);
    }
    if ((conv_file = fopen(conv_file_name, "w")) == NULL)
    {
        fprintf(stderr, "Cannot open convergence file to write! Exiting.\n");
        exit(EXIT_FAILURE);
    }
    //write headers
    //TODO maximal color value has to be in line 3 - & check for return value ?!
    char header_first[3] = "P3\n";
    fwrite(header_first, sizeof(char), 3, attr_file);
    fwrite(header_first, sizeof(char), 3, conv_file);

    char header_second[5];
    sprintf(header_second, "%d %d\n", nmb_lines, nmb_lines);
    fwrite(header_second, sizeof(char), 4, attr_file);
    fwrite(header_second, sizeof(char), 4, conv_file);

    //Attention: 10\n counts as 3 characters - so we have to later on set this value correct
    fwrite("10\n", sizeof(char), 3, attr_file);
    fwrite("3\n", sizeof(char), 2, conv_file);

    char *item_done_loc = (char *)calloc(nmb_lines, sizeof(char));
    for (size_t ix = 0; ix < nmb_lines;)
    {
        pthread_mutex_lock(&item_done_mutex);
        if (item_done[ix] != 0)
            memcpy(item_done_loc, item_done, nmb_lines * sizeof(char));
        pthread_mutex_unlock(&item_done_mutex);

        if (item_done_loc[ix] == 0)
        {
            //TODO try to find "good" value for sleep time - this just prevents us
            //from aquiring the lock over and over again if there is no work
            nanosleep(&sleep2ms, NULL);
            continue;
        }

        for (; ix < nmb_lines && item_done_loc[ix] != 0; ++ix)
        {
//Prior to iterating through the items, prepare strings for each triple of
//color and gray values that you will need. You can either hardcode them or employ sprintf.
//When writing an item (i.e. a row) write the prepared strings directly to file via fwrite.
            res_attr = attractors[ix];
            res_conv = convergences[ix];
            //write line
            for (size_t j = 0; j < nmb_lines; j++)
            {
                //TODO this is just for testing output
                char temp[7];
                sprintf(temp, "%d %d %d ", res_attr[j], res_attr[j], res_attr[j]);
                fwrite(temp, sizeof(char), 6, attr_file);
                sprintf(temp, "%d %d %d ", res_conv[j], res_conv[j], res_conv[j]);
                fwrite(temp, sizeof(char), 6, conv_file);
            }
            fwrite("\n", sizeof(char), 1, attr_file);
            fwrite("\n", sizeof(char), 1, conv_file);

            free(res_attr);
            free(res_conv);
        }
    }
    free(item_done_loc);

    fclose(attr_file);
    fclose(conv_file);

    return NULL;
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

void parseArguments(int argc, char *argv[], char *progname, 
    short unsigned int *nmb_threads, unsigned int *nmb_lines, short unsigned int *poly)
{
    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s -t<threads> -l<rows/columns> <polynomial>\n", progname);
        exit(EXIT_FAILURE);
    }

    int opt;
    while ((opt = getopt(argc, argv, "l:t:")) != -1)
    {
        switch (opt)
        {
        case 't':
            *nmb_threads = convertToInt(optarg);
            break;
        case 'l':
            *nmb_lines = convertToInt(optarg);
            break;
        default:
            fprintf(stderr, "Usage: %s -t<threads> -l<rows/columns> <polynomial>\n", progname);
            exit(EXIT_FAILURE);
        }
    }
    if (optind == 4)
    {
        fprintf(stderr, "Error: no argument for poly was given!\n");
        exit(EXIT_FAILURE);
    }
    *poly = convertToInt(argv[optind]);
    //check for validity of arguments:
    if (*nmb_threads <= 0)
    {
        fprintf(stderr, "Error: Given number of threads is invalid/missing!\n");
        exit(EXIT_FAILURE);
    }
    if (*nmb_lines <= 0 || *nmb_lines > 100000) //100000 is given in the assignment description as upper bound
    {
        fprintf(stderr, "Error: Given number of rows/columns either missing or too low/high!\n");
        exit(EXIT_FAILURE);
    }
    if (*poly >= 10)
    {
        fprintf(stderr, "Error: Poly (%hu) is too high, has to be < 10!\n", *poly);
        exit(EXIT_FAILURE);
    }
}
