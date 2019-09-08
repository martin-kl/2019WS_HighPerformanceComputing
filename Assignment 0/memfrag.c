#include <stdio.h>
#include <stdlib.h>

#define SIZE 10

int main() {
    int counter = 1;

    // Not avoiding memory fragmentation:
    int ** as = (int**) malloc(sizeof(int*) * SIZE);
    for ( size_t ix = 0; ix < SIZE; ++ix )
      as[ix] = (int*) malloc(sizeof(int) * SIZE);

    for ( size_t ix = 0; ix < SIZE; ++ix )
      for ( size_t jx = 0; jx < SIZE; ++jx )
        as[ix][jx] = counter++;

    printf("Printing first and last element of fragmented memory:\n");
    //print first element
    printf("%d\n", as[0][0]);
    //print last element
    printf("%d\n", as[SIZE-1][SIZE-1]);

    for ( size_t ix = 0; ix < SIZE; ++ix )
        free(as[ix]);
    free(as);

    /*
     * Explanation:
     * **as: functs as 2-dimensional array
     *      in declaration line first dimension is malloce'd
     *      for loop malloce'd second dimension
     *  ==> memory NOT guaranteed to be next to each other
     */

    printf("\n");

// ## ## ## ## ## ## ## ## ## ## ## ## ## ## ##

    // Avoiding memory fragmentation:
    counter = 1;
    int * asentries = (int*) malloc(sizeof(int) * SIZE*SIZE);
    int ** as2 = (int**) malloc(sizeof(int*) * SIZE);
    for ( size_t ix = 0, jx = 0; ix < SIZE; ++ix, jx+=SIZE )
      as2[ix] = asentries + jx;

    for ( size_t ix = 0; ix < SIZE; ++ix )
      for ( size_t jx = 0; jx < SIZE; ++jx )
        as2[ix][jx] = counter++;

    printf("Printing first and last element of non-fragmented memory:\n");
    //print first element
    printf("%d\n", as2[0][0]);
    printf("%d\n", as2[SIZE-1][SIZE-1]);

    free(as2);
    free(asentries);

    /*
     * Explanation:
     * *asentries: single pointer to beginning of memory region containing all entries
     * the whole region is malloce'd by one malloc command
     *  -> therefore we know we get one continuous memory region
     *
     *  second malloc only needed for pointers to get the 2-dim array feeling
     *  **as2 is therefore the same as **as but the regions in memory can be different
     */
}
