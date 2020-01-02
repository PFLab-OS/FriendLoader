#!/bin/bash

# Usage: deploy.sh <imgfile>
# insmod + deploy

# errexit, nounset, xtrace
set -eux

test $# -eq 2

# insmod

if lsmod | grep friend_loader; then
	sudo rmmod friend_loader.ko
fi
sudo insmod friend_loader.ko

dev_id=$(cat /proc/devices | grep -E "friend_loader$" | cut -d ' ' -f 1)
dev_id2=$(cat /proc/devices | grep -E "friend_loader2$" | cut -d ' ' -f 1)
if [ -e /dev/friend_loader ]; then
	sudo rm /dev/friend_loader
fi
if [ -e /dev/friend_loader2 ]; then
	sudo rm /dev/friend_loader2
fi
sudo mknod /dev/friend_loader c $dev_id 0
sudo mknod /dev/friend_loader2 c $dev_id2 0

# deploy

cat $1 | sudo dd of=/dev/friend_loader
cat $2 | sudo dd of=/dev/friend_loader2
