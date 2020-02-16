#!/bin/bash

# Usage:
# insmod
# will insert friend_loader and create device files

# errexit, nounset
set -eu

readonly MOD=../FriendLoader/friend_loader.ko

if [[ ! -e ${MOD} ]]; then
	echo 'module not exsits' >&2
	exit 1
fi


# insmod

if lsmod | grep friend_loader; then
	sudo rmmod friend_loader
fi

sudo insmod ${MOD}


# create device files

devid=$(cat /proc/devices | grep -E "friend_loader$" | cut -d ' ' -f 1)
devid2=$(cat /proc/devices | grep -E "friend_loader2$" | cut -d ' ' -f 1)

if [[ -e /dev/friend_loader ]]; then
	sudo rm /dev/friend_loader
fi
if [[ -e /dev/friend_loader2 ]]; then
	sudo rm /dev/friend_loader2
fi

sudo mknod /dev/friend_loader c $devid 0
sudo mknod /dev/friend_loader2 c $devid2 0
