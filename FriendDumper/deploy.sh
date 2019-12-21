#!/bin/bash

# errexit, nounset, xtrace
set -eux

if lsmod | grep --quiet friend_dumper; then
	sudo rmmod friend_dumper.ko
fi
sudo insmod friend_dumper.ko

if [ -e /dev/friend_dumper ]; then
	sudo rm /dev/friend_dumper	
fi
dev_id=$(cat /proc/devices | grep friend_dumper | cut -d ' ' -f 1)
sudo mknod /dev/friend_dumper c $dev_id 0
