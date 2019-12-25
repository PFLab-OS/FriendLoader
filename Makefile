CFLAGS = -Wall -Wextra -Wconversion

IMG := test.bin 
DEPLOY_PHYS_ADDR_START := 0xba700000

.PHONY: run read FORCE clean

run: $(IMG)
	IMG=$(abspath $(IMG)) make -C FriendLoader run
	make -C FriendDumper deploy
	make -C FriendDumper read

read:
	make -C FriendDumper read

#dhry.bin: dhry.fl toimg
#	./toimg dhry.fl dhry.bin
#
#dhry.fl: FORCE
#	make -C ~/dhrystone d
#	cp ~/dhrystone/dhry.fl ./

test.bin: test toimg
	./toimg $< $@

# the order of source files is very important
# because current friendloader ignores ELF's entry point
# and execution will start from top of the first source file
test: entry.s fllib.c test.c
	gcc -o $@ -ffreestanding -nostdlib -s -static \
		-Xlinker -Ttext -Xlinker $(DEPLOY_PHYS_ADDR_START)\
		$^

toimg: toimg.c
	gcc $(CFLAGS) -o $@ $<

clean:
	rm -f test.bin test toimg
	make -C FriendDumper clean
	make -C FriendLoader clean
