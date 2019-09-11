CTime	= time.c

CC		= gcc
CFLAGS	= -O0

.PHONY : clean

all : time

# $@ = target
# $< = source
time :
	$(CC) -Wall $(CFLAGS) $(CTime) -o $@

clean:
	rm -f time
