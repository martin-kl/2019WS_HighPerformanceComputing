Cdistances	= cell_distances.c

CC		= gcc
CFLAGS	= -O2 -Wall

.PHONY : clean clear

all : cell_distances

# $@ = target
# $< = source
cell_distances :
	$(CC) $(CFLAGS) -fopenmp $(Cdistances) -o $@ -lm

# also removes all *.o files
clean:
	rm -f cell_distances $(FILES) *.o

#TODO
#clear:
#	rm -f
