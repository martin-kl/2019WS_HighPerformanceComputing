CTime	= time.c time_diff.c

CInlMain= mul_cpx_mainfile.c
CInlSep	= mul_cpx_separatefile.c mulcpx.c
CInl	= mul_cpx_inlined.c

CLocal	= locality.c time_diff.c

CC		= gcc
CFLAGS	= -O2

.PHONY : clean

all : time mul_cpx_mainfile mul_cpx_separatefile mul_cpx_inlined locality

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

locality :
	$(CC) -Wall $(CFLAGS) $(CLocal) -o $@

time_diff.o : time_diff.c time_diff.h


# also removes all *.o files
clean:
	rm -f time mul_cpx_mainfile *.o mul_cpx_separatefile mul_cpx_inlined locality
