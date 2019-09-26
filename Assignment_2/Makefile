Cnewton	= newton.c

CC		= gcc
CFLAGS	= -O0

.PHONY : clean clear

all : newton

# $@ = target
# $< = source
newton :
	$(CC) -Wall $(CFLAGS) $(Cnewton) -o $@ -lpthread

# also removes all *.o files
clean:
	rm -f newton $(FILES) *.o

clear:
	rm -f newton_*.ppm
