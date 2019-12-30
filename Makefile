CFLAGS = -Wall -Wextra -Wconversion

# configure here to change friend task
IMG := dhry.img 
DEPLOY_PHYS_ADDR_START := 0xba700000

run: $(IMG)
	make -C FriendDumper deploy
	IMG=$(abspath $(IMG)) make -C FriendLoader run
	make -C FriendDumper read

read:
	make -C FriendDumper read

spinkill:
	make -C dhrystone spinkill

test.img: test toimg
	./toimg $< $@

# the order of source files is very important
# because current friendloader ignores ELF's entry point
# and execution will start from top of the first source file
test: entry.s fllib.c test.c
	gcc -o $@ -ffreestanding -nostdlib -s -static \
		-Xlinker -Ttext -Xlinker $(DEPLOY_PHYS_ADDR_START)\
		$^

dhrystone/dhry: FORCE
	make -C dhrystone dhry

dhry.img: dhrystone/dhry toimg
	./toimg $< dhry.img

toimg: toimg.c
	gcc $(CFLAGS) -o $@ $<

clean:
	rm -f test.bin test dhry.img dhry toimg
	make -C dhrystone clean
	make -C FriendDumper clean
	make -C FriendLoader clean

.PHONY: run read FORCE clean

FORCE:
