.PHONY: ready clean

ready:
	make -C toimg toimg
	make -C FriendLoader mod

clean:
	make -C toimg clean
	make -C FriendLoader clean
