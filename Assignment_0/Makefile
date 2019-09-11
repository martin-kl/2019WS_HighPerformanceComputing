CStack	= stack.c
CHeap	= heap.c
CMemf	= memfrag.c
CFiles	= files.c
CArgs	= argparsing.c
FILES	= testfile

CC		= gcc
CFLAGS	= -O1

.PHONY : clean

all : stack heap memfrag files args

# $@ = target
# $< = source
stack :
	$(CC) -Wall $(CFLAGS) $(CStack) -o $@

heap:
	$(CC) -Wall $(CFLAGS) $(CHeap) -o $@

memfrag:
	$(CC) -Wall $(CFLAGS) $(CMemf) -o $@

files:
	$(CC) -Wall $(CFLAGS) $(CFiles) -o $@

args:
	$(CC) -Wall $(CFLAGS) $(CArgs) -o $@

clean:
	rm -f $(FILES) stack heap memfrag files args
