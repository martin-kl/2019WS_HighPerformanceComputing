#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>

#define SIZE 10
#define FILENAME "testfile"

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
		printf("Further characters after number: %s\n", endptr);
        exit(1);
	}
    return number;
}

int main(int argc, char *argv[]) {
    char *progname;
    long a = -1;
    long b = -1;

    if(argc > 0) {
        progname = argv[0];
    }
    if(argc != 3) {
        printf("Usage: %s -aA -bB\n", progname);
        exit(1);
    }

    int opt;
    while((opt = getopt(argc, argv, "a:b:")) != -1) {
        switch(opt) {
            case 'a':
                a = convertToInt(optarg);
                break;
            case 'b':
                b = convertToInt(optarg);
                break;
        }
    }

    printf("A is %ld and B is %ld\n", a, b);
}
