Cnewton	= newton.c
FILES	= 

CC		= gcc
CFLAGS	= -O0

.PHONY : clean

all : newton

# $@ = target
# $< = source
newton :
	$(CC) -Wall $(CFLAGS) $(Cnewton) -o $@ -lpthread

# also removes all *.o files
clean:
	rm -f newton $(FILES) *.o
