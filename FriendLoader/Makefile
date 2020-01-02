obj-m := friend_loader.o
friend_loader-objs := main.o booter.o cpu_hotplug.o deploy.o deploy_dev.o

# this should be set by parent make or your own
# IMG1 :=
# IMG2 :=
# this should be replaced for your environment
KERN_SRC := ~/linux-hikey960

.PHONY: run deploy mod clean

run: deploy
	make -C ../dhrystone spinrun
	echo 1 | sudo tee /sys/module/friend_loader/boot

deploy: mod
	./deploy.sh $(IMG1) $(IMG2)

mod:
	make -C $(KERN_SRC) M=$(CURDIR) modules

clean:
	make -C $(KERN_SRC) M=$(CURDIR) clean
