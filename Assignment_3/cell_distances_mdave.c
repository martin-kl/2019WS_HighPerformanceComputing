#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <omp.h>

#define FILENAME1 "cell_e5.txt"
#define FILENAME "cells.txt"

int main(int argc, char *argv[])
{
    int t = 5;
	omp_set_num_threads(t);

	float ** coordinates = (float **)malloc(sizeof(float)*3466);
	for(short int ix = 0; ix < 3466; ix++) {
		coordinates[ix] = (float *)malloc(sizeof(float) * 2);
	}
	
	FILE *fptr;
	fptr = fopen(FILENAME , "r");
	
	int count = 0;
	for (char c = getc(fptr); c != EOF; c = getc(fptr)) 
        if (c == '\n')
            count = count + 1;
	
	printf("%d\n\n", count);
	
	/* 	fseek(fptr, 0L, SEEK_END);
	int numbytes = ftell(fptr);
	printf("\n%d\n", numbytes);
	fseek(fptr, 0L, SEEK_SET);
	
	char *buffer = (char*)malloc(numbytes*sizeof(char));	
 
	if(buffer == NULL)
		return 1;
	
	fread(buffer, sizeof(char), numbytes, fptr);
	fclose(fptr); */
	
	float x[count];
	float y[count];
	float z[count];
	int i = 0, j = 0;

	fseek(fptr, 0L, SEEK_SET);
	while(i < count) {
		fscanf(fptr, "%f %f %f", &x[i], &y[i], &z[i]);
		/*x[i] *= 1000;
		y[i] *= 1000;
		z[i] *= 1000;*/
		//printf("%f\t%f\t%f\n", x[i], y[i], z[i]);
		i++;
	}
	
	// distance calculation
	
	float d;
	
	/*for(i = 0; i < count; i++) {
		for( j = i+1; j < count; j++) {
			d = ((x[i]-x[j])*(x[i]-x[j]) + (y[i]-y[j])*(y[i]-y[j]) + (z[i]-z[j])*(z[i]-z[j]));
			coordinates[d+1][j] += 1;
		}
	}*/
	
    return 0;
}
