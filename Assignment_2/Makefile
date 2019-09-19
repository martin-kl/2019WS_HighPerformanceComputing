Cnewton	= newton.c
FILES	= 

CC		= gcc
CFLAGS	= -O2

.PHONY : clean

all : newton

# $@ = target
# $< = source
newton :
	$(CC) -Wall $(CFLAGS) $(Cnewton) -o $@

# also removes all *.o files
clean:
	rm -f newton $(FILES)
