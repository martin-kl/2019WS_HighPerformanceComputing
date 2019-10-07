#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <time.h> // for nanosleep
#include <pthread.h>
#include <math.h>
#include <complex.h>

#define M_PI 3.14159265358979323846
#define EPSILON 0.000001 // already squared to milk the miliseconds
#define N_ROOT 9999999999

//--    prototypes              //////////////////////////////////////////////////

void timespec_diff(struct timespec *start, struct timespec *stop, struct timespec *result)
{
    if ((stop->tv_nsec - start->tv_nsec) < 0)
    {
        result->tv_sec = stop->tv_sec - start->tv_sec - 1;
        result->tv_nsec = stop->tv_nsec - start->tv_nsec + 1000000000;
    }
    else
    {
        result->tv_sec = stop->tv_sec - start->tv_sec;
        result->tv_nsec = stop->tv_nsec - start->tv_nsec;
    }
    return;
}

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

/*
 */
static inline double complex compute_next_x(double complex previous_x, double d);

/*
 */
static inline short compute_root(double complex x1, short int *conv);

//--    variables              //////////////////////////////////////////////////

short unsigned int nmb_threads = 0; //number of threads
unsigned int nmb_lines = 0;         //nmb_lines can be 100.000 -> short int would be too small
short unsigned int poly = 0;
double stepping;

// hardcoded roots
static double complex root[9][9] = {
    {1, 0, 0, 0, 0, 0, 0, 0, 0},                                                                                                                                                                                                                                    // roots for x - 1, d = 1
    {1, -1, 0, 0, 0, 0, 0, 0, 0},                                                                                                                                                                                                                                   // roots for x^2 - 1, d = 2
    {1, -0.5 + 0.8660254037 * I, -0.5 - 0.8660254037 * I, 0, 0, 0, 0, 0, 0},                                                                                                                                                                                        // roots for x^3 - 1, d = 3
    {1, -1, 1 * I, -1 * I, 0, 0, 0, 0, 0},                                                                                                                                                                                                                          // roots for x^4 - 1, d = 4
    {1, -0.8090169943 + 0.5877852522 * I, -0.8090169943 - 0.5877852522 * I, 0.3090169943 + 0.9510565162 * I, 0.3090169943 - 0.9510565162 * I, 0, 0, 0, 0},                                                                                                          // roots for x^5 - 1, d = 5
    {1, -1, -0.5 + 0.8660254037 * I, -0.5 - 0.8660254037 * I, 0.5 + 0.8660254037 * I, 0.5 - 0.8660254037 * I, 0, 0, 0},                                                                                                                                             // roots for x^6 - 1, d = 6
    {1, -0.9009688679 + 0.4338837391 * I, -0.9009688679 - 0.4338837391 * I, -0.2225209339 + 0.9749279121 * I, -0.2225209339 - 0.9749279121 * I, 0.6234898018 + 0.7818314824 * I, 0.6234898018 - 0.7818314824 * I, 0, 0},                                            // roots for x^7 - 1, d = 7
    {1, -1, 1 * I, -1 * I, -0.7071067811 + 0.7071067811 * I, -0.7071067811 - 0.7071067811 * I, 0.7071067811 + 0.7071067811 * I, 0.7071067811 - 0.7071067811 * I, 0},                                                                                                // roots for x^8 - 1, d = 8
    {1, -0.9396926207 + 0.3420201433 * I, -0.9396926207 - 0.3420201433 * I, -0.5 + 0.8660254037 * I, -0.5 - 0.8660254037 * I, 0.1736481776 + 0.9848077530 * I, 0.1736481776 - 0.9848077530 * I, 0.7660444431 + 0.6427876096 * I, 0.7660444431 - 0.6427876096 * I}}; // roots for x^9 - 1, d = 9

//TODO check if these data types are correct / perfect for us
// [Martin:] since we have not so many roots char should be enough ?!
short int **attractors;   // roots
short int **convergences; // number iterations

char *item_done;

pthread_mutex_t item_done_mutex = PTHREAD_MUTEX_INITIALIZER;

//--    main()              //////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    struct timespec start;
    struct timespec end;
    struct timespec diff;

    timespec_get(&start, TIME_UTC);

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

    //set divisor:
    stepping = 4 / ((double)nmb_lines - 1);

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

    timespec_get(&end, TIME_UTC);
    timespec_diff(&start, &end, &diff);

    double t1 = diff.tv_sec;
    double t2 = (diff.tv_nsec / 1000000.0);
    double t = t1 * 1000 + t2;

    printf("Time :\t%fms\n", t);

    return (EXIT_SUCCESS);
}

//--    Methods              //////////////////////////////////////////////////

void *compute_main(void *args)
{
    //offset is just the "number" of the thread in our case
    size_t offset = *((size_t *)args);
    free(args);

    double row_imag;
    //double complex x1;
    double complex x0;

    for (size_t row = offset; row < nmb_lines; row += nmb_threads)
    {
        //printf("Starting computation for row %d and compl_row %d\n", row, compl_row);
        short int *attr = (short int *)malloc(sizeof(short int) * nmb_lines); //nmb_lines = nmb_rows
        short int *conv = (short int *)malloc(sizeof(short int) * nmb_lines); //nmb_lines = nmb_rows

        row_imag = 2 - row * stepping;
        x0 = -2 + row_imag * I;
        for (size_t col = 0; col < nmb_lines; ++col)
        {
            //printf("t%ld starting computation for (%0.3f,%0.3fi)\n", offset, creal(x1), cimag(x1));

            short int calculated_root = compute_root(x0, &conv[col]);

            //printf("(%0.3f,%0.3fi)-[%d,%d] converges to root %d\n", creal(x0), cimag(x0), row, col, calculated_root);
            attr[col] = calculated_root;
            //go to next col
            x0 += stepping;
        }

        attractors[row] = attr;
        convergences[row] = conv;

        pthread_mutex_lock(&item_done_mutex);
        item_done[row] = 1;
        pthread_mutex_unlock(&item_done_mutex);
    }
    return NULL;
}

static inline short compute_root(double complex x1, short int *conv)
{
    short int iterations = 0;
    double complex difference;

    while (1)
    {
        //x1 = x0 - (cpow(x0, poly) - 1) / (poly * cpow(x0, poly - 1));
        //printf("#%d - x1: %g, %gi\n", iterations, creal(x1),cimag(x1));
        double realx1 = creal(x1);
        double imagx1 = cimag(x1);

        if ((realx1 * realx1 + imagx1 * imagx1) <= (1 + EPSILON))
        {
            for (size_t ix = 0; ix < poly; ix++)
            {
                difference = x1 - root[poly - 1][ix];
                if ((creal(difference) * creal(difference) + cimag(difference) * cimag(difference)) <= EPSILON)
                { // trying not to use cabs()
                    //printf("this point converges to root number %ld = %.15f + %.15f i\n",ix + 1,creal(root[poly-1][ix]),cimag(root[poly-1][ix]));
                    //attr[col] = ix;
                    *conv = iterations < 100 ? iterations : 99;
                    return ix;
                }
            }
            if ((realx1 * realx1 + imagx1 * imagx1) <= EPSILON)
            { //trying not to use cabs()
                //printf("special case x1 tends to 0\n");
                //write maximal 99 otherwise ppm file would be wrong with max value 100
                *conv = iterations < 100 ? iterations : 99;
                return 9;
            }
        }
        if (realx1 > N_ROOT || realx1 < -N_ROOT || imagx1 > N_ROOT || imagx1 < -N_ROOT)
        { //trying not to use cabs()
            //printf("special case x1 >= 10000000000\n");
            *conv = iterations < 100 ? iterations : 99;
            return 10;
        }

        x1 = compute_next_x(x1, poly);
        iterations++;
    }
}

static inline double complex compute_next_x(double complex previous_x, double d)
{
    double real, imag, add_for_correction, r, theta;

    real = creal(previous_x);
    imag = cimag(previous_x);
    add_for_correction = 0;

    if (real < 0)
        add_for_correction = M_PI;
    else if (imag < 0)
        add_for_correction = 2 * M_PI;

    r = sqrt(real * real + imag * imag);
    theta = atan(imag / real) + add_for_correction;

    return (previous_x - (((pow(r, d) * (cos(d * theta) + I * sin(d * theta))) - 1) / (d * (pow(r, (d - 1)) * (cos((d - 1) * theta) + I * sin((d - 1) * theta))))));
}

void *write_method(void *args)
{
    FILE *attr_file;
    FILE *conv_file;
    struct timespec sleep1ms = {0, 1000000L}; // 1 ms

    short int *res_attr; //used to point to current row to handle
    short int *res_conv; //used to point to current row to handle

    int attractor_chars, convergence_chars;

    // ---- ---- ---- color values for attractors ---- ---- ----
    char *colors[11] = {
        "5 0 0 ",
        "0 5 0 ",
        "0 0 5 ",
        "5 5 0 ",
        "5 0 5 ",
        "0 5 5 ",
        "2 3 0 ",
        "0 3 2 ",
        "2 3 3 ",
        "0 0 0 ", //black, converging to 0
        "5 5 5 "  //white, converging to inf
    };

    // ---- ---- ---- create / open both files ---- ---- ----

    char buffer[128]; //we need 25 chars since sprintf automatically null terminates!!
    sprintf(buffer, "newton_attractors_x%hu.ppm", poly);
    if ((attr_file = fopen(buffer, "w")) == NULL)
    {
        fprintf(stderr, "Cannot open attractor file to write! Exiting.\n");
        exit(EXIT_FAILURE);
    }
    sprintf(buffer, "newton_convergence_x%hu.ppm", poly);
    if ((conv_file = fopen(buffer, "w")) == NULL)
    {
        fprintf(stderr, "Cannot open convergence file to write! Exiting.\n");
        exit(EXIT_FAILURE);
    }

    // ---- ---- ---- write headers of ppm files ---- ---- ----
    attractor_chars = sprintf(buffer, "P3\n%d %d\n%d\n", nmb_lines, nmb_lines, 5); //color image
    fwrite(buffer, sizeof(char), attractor_chars, attr_file);
    convergence_chars = sprintf(buffer, "P3\n%d %d\n%d\n", nmb_lines, nmb_lines, 100); //grayscale TODO: in example ppm file the max color is 100 ?!
    fwrite(buffer, sizeof(char), convergence_chars, conv_file);

    // ---- ---- ---- write rows ---- ---- ----

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
            nanosleep(&sleep1ms, NULL);
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
                //printf("\t # [%d,%d] root %lu\n", ix, j, res_attr[j]);
                fwrite(colors[res_attr[j]], sizeof(char), 6, attr_file);
                convergence_chars = sprintf(buffer, "%03d %03d %03d ", res_conv[j], res_conv[j], res_conv[j]);
                fwrite(buffer, sizeof(char), convergence_chars, conv_file);
            }
            //printf("Wrote line\n");
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
