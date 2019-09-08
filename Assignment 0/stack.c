#include <stdio.h>

#define SIZE 2094910
// #define SIZE 2094911 //is too much
// an int-array with 2094911 entries (size) is simply
// too large for the stack

int main() {
    int as[SIZE];
    for ( size_t ix = 0; ix < SIZE; ++ix ) {
      as[ix] = 0;
    }
    printf("%d\n", as[0]);
}
