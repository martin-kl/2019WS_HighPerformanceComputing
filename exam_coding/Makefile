CC		= gcc
CFLAGS	= -O2 -Wall
# CFLAGS	= -O2 -Wall -march=native

.PHONY : clean

all : exam_naive exam_seq exam_par

# $@ = target
# $< = source
exam_naive :
	$(CC) $(CFLAGS) exam_naive.c -o $@

exam_seq :
	$(CC) $(CFLAGS) exam_sequential.c -o $@

exam_par :
	$(CC) $(CFLAGS) exam_parallelized.c -o $@ -fopenmp

# also removes all *.o files
clean:
	rm -f exam_naive exam_seq exam_par *.o
