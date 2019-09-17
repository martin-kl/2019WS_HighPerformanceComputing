#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define SIZE 10
#define FILENAME "testfile"

int main() {
    //initialize array
    int * array = (int*) malloc(sizeof(int) * SIZE*SIZE);
    int ** as = (int**) malloc(sizeof(int*) * SIZE);
    for ( size_t ix = 0, jx = 0; ix < SIZE; ++ix, jx+=SIZE )
      as[ix] = array + jx;


    FILE *fp;
    if( (fp=fopen(FILENAME, "w")) == NULL) {
        fprintf(stderr, "Cannot open file to write! Exiting.\n");
        exit(1);
    }

    int counter = 1;
    for(int i = 0; i < SIZE; i++) {
        for(int j = 0; j < SIZE; j++) {
            as[i][j] = counter++;
            if(fprintf(fp, "%d ", as[i][j]) <= 0) {
                fprintf(stderr, "Error writing to file...\n");
            }
        }
    }
    // flushing not needed since fclose does it too
    //fflush(fp);
    fclose(fp);





    //reopen file for reading:
    if( (fp = fopen(FILENAME, "r")) == NULL) {
        printf("Cannot open file to read it! Exiting.\n");
        exit(1);
    }
    //initialize second array
    int * a2 = (int*) malloc(sizeof(int) * SIZE*SIZE);
    int ** as2 = (int**) malloc(sizeof(int*) * SIZE);
    for ( size_t ix = 0, jx = 0; ix < SIZE; ++ix, jx+=SIZE )
        as2[ix] = a2 + jx;

    int i = 0;
    int j = 0;
    while(fscanf(fp, "%d", &as2[i][j]) == 1) {
        //printf("found: %d\n", as2[i][j]);
        assert(as2[i][j] == as[i][j]);
        j++;
        if(j == SIZE) {
            i++;
            j = 0;
        }
    }

    printf("Everything checked out to be fine, all numbers are equal!\n");

    fclose(fp);
}
