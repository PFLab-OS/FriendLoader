obj-m := friend_loader.o
friend_loader-objs := main.o flsysfs.o cpu_hotplug.o deploy.o deploy_dev.o

# this should be replaced for your environment
KERN_SRC := ~/linux-hikey960

.PHONY: deploy mod clean

mod:
	make -C $(KERN_SRC) M=$(CURDIR) modules

clean:
	make -C $(KERN_SRC) M=$(CURDIR) clean
