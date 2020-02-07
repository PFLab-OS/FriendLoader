CFLAGS = -Wall -Wextra -Wconversion

LIBDIR := ../fllib
LIBS := $(LIBDIR)/entry.S $(LIBDIR)/fllib.c

TOIMG := ../toimg/toimg

# include DEPLOY_PADDR1/2
include ../common.inc

# change if necesarry
NAME := sample
SRC := $(NAME).c
ELFS := $(NAME)1.elf $(NAME)2.elf
IMGS := $(NAME)1.img $(NAME)2.img

.PHONY: run

.PRECIOUS: $(ELFS)

run: $(IMGS)
	./run.sh $(IMGS)

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

clean:
	$(RM) $(ELFS) $(IMGS)
