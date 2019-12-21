.PHONY: run read FORCE clean

IMG := abcd.bin

run: $(IMG)
	IMG=$(abspath $(IMG)) make -C FriendLoader run
	make -C FriendDumper deploy
	make -C FriendDumper read

read:
	make -C FriendDumper read

#dhry.bin: dhry.fl ELFtoImg/main
#	ELFtoImg/main dhry.fl dhry.bin

#dhry.fl: FORCE
#	cp ../dhrystone/dhry.fl ./

abcd.bin: abcd
	make -C ELFtoImg
	ELFtoImg/main abcd abcd.bin

abcd: abcd.s
	gcc -o abcd -ffreestanding -nostdlib -s -static\
		-Xlinker -Ttext -Xlinker 0x0\
	       	abcd.s

#test: test.c
#	gcc -o test -ffreestanding -nostdlib -static test.c

clean:
	rm -f abcd abcd.bin
	make -C FriendDumper clean
	make -C FriendLoader clean
