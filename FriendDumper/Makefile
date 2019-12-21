obj-m := friend_dumper.o
friend_dumper-objs := main.o dumper_dev.o

KERN_SRC := ~/linux-hikey960

.PHONY: read deploy mod clean

# trigger FriendDumper read
# it assumes that `make deploy` already done
read: reader
	./reader

# if old kernel module is already deployed, it will be automatically removed
deploy:
	./deploy.sh

mod:
	make -C $(KERN_SRC) M=$(PWD) modules

clean:
	rm -f reader
	make -C $(KERN_SRC) M=$(PWD) clean
