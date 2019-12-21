#!/bin/bash

# Usage: run.sh <file>
#
# `file` is deployed if specified.

# errexit, nounset, xtrace
set -eux

test $# -eq 1

if lsmod | grep friend_loader; then
	sudo rmmod friend_loader.ko
fi
sudo insmod friend_loader.ko

dev_id=$(cat /proc/devices | grep friend_loader | cut -d ' ' -f 1)
if [ -e /dev/friend_loader ]; then
	sudo rm /dev/friend_loader
fi
sudo mknod /dev/friend_loader c $dev_id 0

cat $1 | sudo dd of=/dev/friend_loader
