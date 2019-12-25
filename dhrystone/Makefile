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
#TIME_FUNC=	-DTIME			# Use time(2) for measurement
TIME_FUNC=	-DTIMES			# Use times(2) for measurement
#HZ=		50			# Frequency of times(2) clock ticks
HZ=		60			# Frequency of times(2) clock ticks
#HZ=		100			# Frequency of times(2) clock ticks
#HZ=		1			# Give bogus result unless changed!

STRUCTASSIGN=	-DNOSTRUCTASSIGN	# Compiler cannot assign structs
STRUCTASSIGN=				# Compiler can assign structs

ENUMS=		-DNOENUMS		# Compiler doesn't have enum type
ENUMS=					# Compiler does have enum type

GCCOPTIM=       -O

LFLAGS=					#Loader Flags

GCCFLAGS= $(GCCOPTIM) $(TIME_FUNC) -DHZ=$(HZ) $(ENUMS) $(STRUCTASSIGN) $(CFL)

#
#		You shouldn't need to touch the rest
#
SRC=		dhry_1.c dhry_2.c
HDR=		dhry.h

UNIX_PROGS=	gcc_dry2 gcc_dry2reg

#all:	$(PROGS)
#
#unix:	$(UNIX_PROGS)
#
#gcc_dry2:		$(SRC) $(HDR)
#		$(GCC) $(GCCFLAGS) $(SRC) $(LFLAGS) -o $@
#
#gcc_dry2reg:	$(SRC) $(HDR)
#		$(GCC) $(GCCFLAGS) -DREG=register $(SRC) $(LFLAGS) -o $@

clean:
	-rm -f *.o *.obj

clobber: clean
	-rm -f $(UNIX_PROGS)
