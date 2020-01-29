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
#TIME_FUNC=	-DMSC_CLOCK		# Use Microsoft clock() for measurement
#TIME_FUNC=	-DTIME			# Use time(2) for measurement
#TIME_FUNC=	-DTIMES			# Use times(2) for measurement
TIME_FUNC=	-DCLOCK

# value of HZ is obtained from 'make gethz'
# but now time() is used and HZ is irrelevant to result
#HZ=		50			# Frequency of times(2) clock ticks
#HZ=		60			# Frequency of times(2) clock ticks
HZ=		100			# Frequency of times(2) clock ticks
#HZ=		1			# Give bogus result unless changed!

#STRUCTASSIGN=	-DNOSTRUCTASSIGN	# Compiler cannot assign structs
STRUCTASSIGN=				# Compiler can assign structs

#ENUMS=		-DNOENUMS		# Compiler doesn't have enum type
ENUMS=					# Compiler does have enum type

#MULTIPROC=	-DMULTIPROC
MULTIPROC=

#OPTIM=       -O
OPTIM=

CFLAGS= $(TIME_FUNC) -DHZ=$(HZ) $(ENUMS) $(STRUCTASSIGN) $(MULTIPROC) $(OPTIM)

# include LIBS
LIBDIR := ../fllib
LIBS := $(LIBDIR)/entry.S $(LIBDIR)/fllib.c

TOIMG := ../toimg/toimg
DEPLOY_PADDR1 := 0xba700000
DEPLOY_PADDR2 := 0xbc700000

#NAME := dhry
NAME := readtask
SRC := $(NAME).c
ELFS := $(NAME)1.elf $(NAME)2.elf
IMGS := $(NAME)1.img $(NAME)2.img

.PHONY:	run linuxrun friendrun clean

.PRECIOUS: $(ELFS)

run: friendrun

linuxrun: $(NAME)
	./linuxrun.sh $(NAME)

friendrun: $(IMGS)
	./friendrun.sh $(IMGS)

# to create $(NAME)1.img or $(NAME)2.img
# $* is 1 or 2 (corresponds to source elf)
$(NAME)%.img: $(NAME)%.elf
	$(TOIMG) $< $@ $*	

# the order of source files is very importnt (entry.S should be first)
$(NAME)%.elf: $(LIBS) $(SRC)
	gcc -o $@ $(CFLAGS) \
	       	-ffreestanding -nostdlib -s -static \
		-Xlinker -Ttext -Xlinker $(DEPLOY_PADDR$*) \
		-DFRIEND -DTASK$* \
		$^

$(NAME): $(SRC)
	gcc -o $@ $(CFLAGS) -static $(SRC)

$(NAME)reg: $(SRC)
	gcc -o $@ $(CFLAGS) -DREG=register -static $(SRC)

clean:
	rm -f *.o *.obj $(NAME) $(ELFS) $(IMGS)
