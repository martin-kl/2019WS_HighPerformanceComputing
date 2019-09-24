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
void parseArguments(int argc, char *argv[], char *progname, short int *t, int*l, short int *poly);

/*
 */
void * compute_main(void * args);

/*
 */
void * write_method(void * args);




short int t = 0; //number of threads
int l = 0; //l can be 100.000 -> short int would be too small
short int poly = 0;

//TODO change to needed types?
int * results;
//int ** results;
char * item_done;

struct timespec sleep100ms = {0, 100000000L}; // 100 ms
pthread_mutex_t item_done_mutex;

//--    main()              //////////////////////////////////////////////////

int main(int argc, char *argv[]) {
    char *progname;

    pthread_t *compute_threads;
    pthread_t write_thread;

    //get program name
    if(argc > 0) {
        progname = argv[0];
    } else {
        fprintf(stderr, "Error: no program name can be found\n");
        exit(1);
    }
    parseArguments(argc, argv, progname, &t, &l, &poly);
    printf("T is %d, L is %d and poly is %d\n", t, l, poly);


    //malloc memory for writing
    results = malloc(sizeof(int) * l);
    item_done = calloc(l, sizeof(char)); //use calloc here so it is for sure 0


    //create t compute threads and one writing thread
    compute_threads = (pthread_t*)malloc(sizeof(pthread_t) * t);
    for ( size_t tx = 0; tx < t; ++tx ) {
        printf("creating thread %ld\n", tx);
        size_t * args = malloc(sizeof(size_t));
        *args = tx;
        pthread_create(&compute_threads[tx], NULL, compute_main, (void*)args);
    }
    pthread_create(&write_thread, NULL, write_method, NULL);

    //joining threads again...
    int ret;
    for (size_t tx=0; tx < t; ++tx) {
        if ((ret = pthread_join(compute_threads[tx], NULL))) {
            printf("Error joining one of the compute threads: %d\n", ret);
            exit(1);
        }
    }
    if ((ret = pthread_join(write_thread, NULL))) {
        printf("Error joining write thread: %d\n", ret);
        exit(1);
    }
    pthread_mutex_destroy(&item_done_mutex);

    //free variables again
    free(results);
    free(item_done);
    free(compute_threads);
}


//--    Methods              //////////////////////////////////////////////////


void * compute_main(void * args) {
    //offset is just the "number" of the thread in our case
    size_t offset = *((size_t*)args);
    free(args);

    for ( size_t ix = offset; ix < l; ix += t ) {
        //printf("\tThread %ld calculates number for row %ld\n", offset, ix);

        //example code from slides:
        //int * result = (int*)malloc(sizeof(int)*item_size);
        // compute work item
        results[ix] = ix;

        //just for now sleep a little bit
        nanosleep(&sleep100ms, NULL);

        pthread_mutex_lock(&item_done_mutex);
        item_done[ix] = 1;
        pthread_mutex_unlock(&item_done_mutex);
    }
    return NULL;
}


void * write_method(void * args){
    struct timespec sleep2ms = {0, 2000000L}; // 2 ms
    //TODO implement - first open file and write header
    // from slides:
    //char * item_done_loc = (char*)calloc(nmb_items, sizeof(char));
    char * item_done_loc = (char*)calloc(l, sizeof(char));
    for ( size_t ix = 0; ix < l; ) {
        pthread_mutex_lock(&item_done_mutex);
        if ( item_done[ix] != 0 )
            memcpy(item_done_loc, item_done, l*sizeof(char));
        pthread_mutex_unlock(&item_done_mutex);

        if ( item_done_loc[ix] == 0 ) {
            //TODO why is this sleep here needed? to avoid deadlock ?
            nanosleep(&sleep2ms, NULL);
            continue;
        }

        for ( ; ix < l && item_done_loc[ix] != 0; ++ix ) {
            int result = results[ix];
            // TODO write result
            printf("%d ", result);
            //not needed for result in this case
            //free(result);
        }
    }
    free(item_done_loc);
    return NULL;
}


long convertToInt(char *arg) {
    char *endptr;
    long number;

    //10 = decimal system, endptr is to check if strtol gave us a number
    number = strtol(arg, &endptr, 10);

	if ((errno == ERANGE && (number == LONG_MAX || number == LONG_MIN))
			|| (errno != 0 && number == 0)) {
        printf("Failed to convert input to number!\n");
		exit(1);
	}
	if (endptr == arg) {
        printf("No digits where found!\n");
		exit(1);
	}

	/* If we got here, strtol() successfully parsed a number */
	if (*endptr != '\0') { /* In principle not necessarily an error... */
		printf("Attention: further characters after number: %s\n", endptr);
        exit(1);
	}
    return number;
}

void parseArguments(int argc, char *argv[], char *progname, short int *t, int*l, short int *poly) {
    if(argc != 4) {
        printf("Usage: %s -t<threads> -l<rows/columns> <polynomial>\n", progname);
        exit(1);
    }

    int opt;
    while((opt = getopt(argc, argv, "t:l:")) != -1) {
        switch(opt) {
            case 't':
                *t = convertToInt(optarg);
                break;
            case 'l':
                *l = convertToInt(optarg);
                break;
        }
    }
    if(optind == 4) {
        printf("Error: no argument for poly was given!\n");
        exit(1);
    }
    *poly = convertToInt(argv[optind]);

    //check for validity of arguments:
    if(*t <= 0) {
        printf("Error: Given number of threads is invalid/missing!\n");
        exit(1);
    }
    if(*l <= 0 || *l > 100000) { //100000 is given in the assignment description as upper bound
        printf("Error: Given number of rows/columns either missing or too low/high!\n");
        exit(1);
    }
}
