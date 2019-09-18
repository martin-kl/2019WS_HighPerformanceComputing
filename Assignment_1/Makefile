CTime	= time.c time_diff.c

CInlMain= mul_cpx_mainfile.c
CInlSep	= mul_cpx_separatefile.c mulcpx.c
CInl	= mul_cpx_inlined.c

CLocal	= locality.c time_diff.c
CAdress	= adressing.c time_diff.c
Chddssd	= hddssd.c time_diff.c
FILES	= hdd_testfile /run/mount/scratch/hpcuser046/ssd_testfile
Cvalgr	= valgrind.c
Cgdb	= gdb_invalid_access.c

CC		= gcc
CFLAGS	= -O0

.PHONY : clean

all : time mul_cpx_mainfile mul_cpx_separatefile mul_cpx_inlined locality adressing hddssd valgr

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

adressing :
	$(CC) -Wall $(CFLAGS) $(CAdress) -o $@

hddssd :
	$(CC) -Wall $(CFLAGS) $(Chddssd) -o $@

valgr :
	$(CC) -Wall $(CFLAGS) $(Cvalgr) -o $@ -g
# -g for valgrind to include debug info

gdb_invalid_access :
	$(CC) -Wall $(CFLAGS) $(Cgdb) -o $@ -g

time_diff.o : time_diff.c time_diff.h


# also removes all *.o files
clean:
	rm -f time mul_cpx_mainfile *.o mul_cpx_separatefile mul_cpx_inlined locality adressing hddssd $(FILES) valgr gdb_invalid_access
