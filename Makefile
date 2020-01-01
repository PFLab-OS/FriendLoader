CFLAGS = -Wall -Wextra -Wconversion

# configure here to change friend task
IMG1 := dhry1.img 
IMG2 := dhry2.img 
DEPLOY_PHYS_ADDR_START := 0xba700000

run: $(IMG1) $(IMG2)
	make -C FriendDumper deploy
	IMG1=$(abspath $(IMG1)) IMG2=$(abspath $(IMG2)) make -C FriendLoader run
	make -C FriendDumper read

read:
	make -C FriendDumper read

spinkill:
	make -C dhrystone spinkill

# test.img: test toimg
# 	./toimg $< $@

# the order of source files is very important
# because current friendloader ignores ELF's entry point
# and execution will start from top of the first source file
# test: entry.s fllib.c test.c
# 	gcc -o $@ -ffreestanding -nostdlib -s -static \
# 		-Xlinker -Ttext -Xlinker $(DEPLOY_PHYS_ADDR_START)\
# 		$^

dhry1.img: dhrystone/dhry1 toimg
	./toimg $< $@ 1

dhry2.img: dhrystone/dhry2 toimg
	./toimg $< $@ 2

dhrystone/dhry1: FORCE
	make -C dhrystone dhry1

dhrystone/dhry2: FORCE
	make -C dhrystone dhry2

toimg: toimg.c
	gcc $(CFLAGS) -o $@ $<

clean:
	rm -f test.bin test dhry1.img dhry2.img toimg
	make -C dhrystone clean
	make -C FriendDumper clean
	make -C FriendLoader clean

.PHONY: run read spinkill FORCE clean

FORCE:
