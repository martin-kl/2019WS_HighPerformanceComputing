Cnewton	= newton.c

CC		= gcc
CFLAGS	= -O2

.PHONY : clean clear

all : newton

# $@ = target
# $< = source
newton :
	$(CC) -Wall $(CFLAGS) -march=native $(Cnewton) -o $@ -lpthread -lm

# also removes all *.o files
clean:
	rm -f newton $(FILES) *.o

clear:
	rm -f newton_*.ppm
