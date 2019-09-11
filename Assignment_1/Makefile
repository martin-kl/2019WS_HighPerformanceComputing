CTime	= time.c
CInlMain= mul_cpx_mainfile.c
CInlSep	= mul_cpx_separatefile.c mulcpx.c
CInl	= mul_cpx_inlined.c

CC		= gcc
CFLAGS	= -O2

.PHONY : clean

all : time mul_cpx_mainfile mul_cpx_separatefile mul_cpx_inlined

# $@ = target
# $< = source
time :
	$(CC) -Wall $(CFLAGS) $(CTime) -o $@

mul_cpx_mainfile :
	$(CC) -Wall $(CFLAGS) $(CInlMain) -o $@

mul_cpx_separatefile : 
	$(CC) -Wall $(CFLAGS) $(CInlSep) -o $@

mul_cpx_inlined :
	$(CC) -Wall $(CFLAGS) $(CInl) -o $@

# mulcpx in separate file:
mulcpx.o : mulcpx.c mulcpx.h

# also removes all *.o files
clean:
	rm -f time mul_cpx_mainfile *.o mul_cpx_separatefile mul_cpx_inlined
