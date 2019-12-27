CFLAGS = -Wall -Wextra -Wconversion

# configure here to change friend task
IMG := dhry.img 
DEPLOY_PHYS_ADDR_START := 0xba700000

.PHONY: run read FORCE clean

run: $(IMG)
	IMG=$(abspath $(IMG)) make -C FriendLoader run
	make -C FriendDumper deploy
	make -C FriendDumper read

read:
	make -C FriendDumper read

test.img: test toimg
	./toimg $< $@

# the order of source files is very important
# because current friendloader ignores ELF's entry point
# and execution will start from top of the first source file
test: entry.s fllib.c test.c
	gcc -o $@ -ffreestanding -nostdlib -s -static \
		-Xlinker -Ttext -Xlinker $(DEPLOY_PHYS_ADDR_START)\
		$^

# for GCCFLAGS for dhry
include dhrystone/Makefile

dhry.img: dhry toimg
	./toimg dhry dhry.img

dhry: entry.s fllib.c dhrystone/dhry.c
	gcc -o $@ -ffreestanding -nostdlib -s -static \
		-Xlinker -Ttext -Xlinker $(DEPLOY_PHYS_ADDR_START)\
		$(GCCFLAGS) -DFRIEND\
		$^

toimg: toimg.c
	gcc $(CFLAGS) -o $@ $<

clean:
	rm -f test.bin test dhry.img dhry toimg
	make -C dhrystone clobber
	make -C FriendDumper clean
	make -C FriendLoader clean
