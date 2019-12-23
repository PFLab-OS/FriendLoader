.PHONY: run read FORCE clean

IMG := test.bin 
DEPLOY_PHYS_ADDR_START := 0xba700000

run: $(IMG)
	IMG=$(abspath $(IMG)) make -C FriendLoader run
	make -C FriendDumper deploy
	make -C FriendDumper read

read:
	make -C FriendDumper read

#dhry.bin: dhry.fl ELFtoImg/main
#	ELFtoImg/main dhry.fl dhry.bin
#
#dhry.fl: FORCE
#	make -C ~/dhrystone d
#	cp ~/dhrystone/dhry.fl ./

test.bin: test
	make -C ELFtoImg
	ELFtoImg/main $< $@

# the order of source files is very important
# because current friendloader ignores ELF's entry point
# and execution will start from top of the first source file
test: entry.s fllib.c test.c
	gcc -o $@ -ffreestanding -nostdlib -s -static \
		-Xlinker -Ttext -Xlinker $(DEPLOY_PHYS_ADDR_START)\
		$^

clean:
	rm -f test.bin test
	make -C FriendDumper clean
	make -C FriendLoader clean
