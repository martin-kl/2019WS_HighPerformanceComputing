CStack	= stack.c
CHeap	= heap.c
CMemf	= memfrag.c
CC		= gcc
CFLAGS	= -O1

.PHONY : clean

all : stack heap memfrag

# $@ = target
# $< = source
stack :
	$(CC) -Wall $(CFLAGS) $(CStack) -o $@

heap:
	$(CC) -Wall $(CFLAGS) $(CHeap) -o $@

memfrag:
	$(CC) -Wall $(CFLAGS) $(CMemf) -o $@

clean:
	rm -f stack heap memfrag
