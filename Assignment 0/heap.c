#include <stdio.h>
#include <stdlib.h>

#define SIZE 2099999
// does not fail as does the stack allocation!

int main() {
    int * as = (int*) malloc(sizeof(int) * SIZE);
    for ( size_t ix = 0; ix < SIZE; ++ix )
      as[ix] = 0;

    printf("%d\n", as[0]);

    free(as);
}
