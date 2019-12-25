#
#	Adjust for your system!
#
#	Common options for generic UNIX and Microsoft C (under DOS)
#	are listed here.  You can change them by switching the order,
#	placing the ones you want last.  Pay particular attention to
#	the HZ parameter, which may or may not be listed in some
#	header file on your system, such as <sys/param.h> or <limits.h>
#	(as CLK_TCK).  Even if it is listed, it may be incorrect.
#	Also, some operating systems (notably some (all?) versions
#	of Microport UNIX) lie about the time.  Sanity check with a
#	stopwatch.
#
#	For Microsoft C under DOS, you need a real make, not MSC make,
#	to run this Makefile.  The public domain "ndmake" will suffice.
#
GCC=		gcc

PROGS=		unix			# Programs to build (UNIX)

#TIME_FUNC=	-DMSC_CLOCK		# Use Microsoft clock() for measurement
TIME_FUNC=	-DTIME			# Use time(2) for measurement
#TIME_FUNC=	-DTIMES			# Use times(2) for measurement

# value of HZ is obtained from 'make gethz'
#HZ=		50			# Frequency of times(2) clock ticks
#HZ=		60			# Frequency of times(2) clock ticks
HZ=		100			# Frequency of times(2) clock ticks
#HZ=		1			# Give bogus result unless changed!

#STRUCTASSIGN=	-DNOSTRUCTASSIGN	# Compiler cannot assign structs
STRUCTASSIGN=				# Compiler can assign structs

#ENUMS=		-DNOENUMS		# Compiler doesn't have enum type
ENUMS=					# Compiler does have enum type

GCCOPTIM=       -O

LFLAGS=					#Loader Flags

GCCFLAGS= $(GCCOPTIM) $(TIME_FUNC) -DHZ=$(HZ) $(ENUMS) $(STRUCTASSIGN) $(CFL)

#
#		You shouldn't need to touch the rest
#
SRC=		dhry.c

UNIX_PROGS=	gcc_dry2 gcc_dry2reg

.PHONY:	all unix gethz

# memo:
# when run dhrystone, goot number of run is >10000000
# because if not, dhrystone says that it is too small
# 
# memo2:
# current result is
# ---
# Microseconds for one run through Dhrystone:    0.2 
# Dhrystones per Second:                      5000000.0 
# ---

all:	$(PROGS)

unix:	$(UNIX_PROGS)

gcc_dry2:	$(SRC)
		$(GCC) $(GCCFLAGS) $(SRC) $(LFLAGS) -o $@

gcc_dry2reg:	$(SRC)
		$(GCC) $(GCCFLAGS) -DREG=register $(SRC) $(LFLAGS) -o $@

gethz:	gethz.c
	gcc -o gethz gethz.c
	./gethz
	rm gethz

clean:
	-rm -f *.o *.obj

clobber: clean
	-rm -f $(UNIX_PROGS)
