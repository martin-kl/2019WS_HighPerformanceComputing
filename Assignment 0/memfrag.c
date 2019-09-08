#include <stdio.h>
#include <stdlib.h>

#define SIZE 10

int main() {
    // Not avoiding memory fragmentation:
    int ** as = (int**) malloc(sizeof(int*) * SIZE);
    for ( size_t ix = 0; ix < SIZE; ++ix )
      as[ix] = (int*) malloc(sizeof(int) * SIZE);

    for ( size_t ix = 0; ix < SIZE; ++ix )
      for ( size_t jx = 0; jx < SIZE; ++jx )
        as[ix][jx] = 0;

    printf("%d\n", as[0][0]);

    for ( size_t ix = 0; ix < SIZE; ++ix )
        free(as[ix]);
    free(as);

    // Avoiding memory fragmentation:
    int * asentries = (int*) malloc(sizeof(int) * SIZE*SIZE);
    int ** as2 = (int**) malloc(sizeof(int*) * SIZE);
    for ( size_t ix = 0, jx = 0; ix < SIZE; ++ix, jx+=SIZE )
      as2[ix] = asentries + jx;

    for ( size_t ix = 0; ix < SIZE; ++ix )
      for ( size_t jx = 0; jx < SIZE; ++jx )
        as2[ix][jx] = 0;

    printf("%d\n", as2[0][0]);

    free(as2);
    free(asentries);
}
