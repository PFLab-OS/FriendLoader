obj-m := friend_loader.o
friend_loader-objs := main.o cpu_hotplug.o deploy.o deploy_dev.o

# this should be set by parent make or your own
# IMG := 
# this should be replaced for your environment
KERN_SRC := ~/linux-hikey960

.PHONY: run deploy mod clean

run:
	make deploy
	echo 1 | sudo tee /sys/module/friend_loader/boot/boot

deploy:
	make mod
	./deploy.sh $(IMG)

mod:
	make -C $(KERN_SRC) M=$(CURDIR) modules

clean:
	make -C $(KERN_SRC) M=$(CURDIR) clean
