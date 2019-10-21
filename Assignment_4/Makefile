LIB			= /usr/local/cuda-10.0/targets/x86_64-linux/lib
Cdistances	= heat_diffusion.c

CC		= gcc
CFLAGS	= -O2 -Wall -march=native

.PHONY : clean

all : heat_diffusion

# $@ = target
# $< = source
heat_diffusion :
	$(CC) $(CFLAGS) $(Cdistances) -o $@ -L$(LIB) -lOpenCL

# also removes all *.o files
clean:
	rm -f heat_diffusion *.o
