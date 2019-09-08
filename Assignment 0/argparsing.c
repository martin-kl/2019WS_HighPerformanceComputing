#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#define SIZE 10
#define FILENAME "testfile"


int main(int argc, char *argv[]) {
    char *a;
    char *b;

    //not yet done
    if(argc != 3) {
        printf("3 arguments required, usage: ./argparsing -aA -bB\n");
        exit(1);
    }

    int opt;
    while((opt = getopt(argc, argv, "a:b:")) != -1) {
        switch(opt) {
            case 'a':
                a = optarg;
                break;
            case 'b':
                b = optarg;
                break;
        }
    }
    //TODO convert numbers to int
    printf("A is %s and B is %s\n", a, b);
}
