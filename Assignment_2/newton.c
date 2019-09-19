#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>

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
    }}

int main(int argc, char *argv[]) {
    char *progname;
    short int t = 0;
    int l = 0; //l can be 100.000 -> short int would be too small
    short int poly = 0;

    //get program name
    if(argc > 0) {
        progname = argv[0];
    } else {
        fprintf(stderr, "Error: no program name can be found\n");
        exit(1);
    }
    parseArguments(argc, argv, progname, &t, &l, &poly);
    printf("T is %d, L is %d and poly is %d\n", t, l, poly);
}
