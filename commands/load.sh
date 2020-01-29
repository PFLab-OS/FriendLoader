#!/bin/bash

# Usage:
# load <img1> <img2>
# will load img1 and img2 to physical address region reserved for FriendLoader

# errexit, nounset
set -eu

if [[ $# -ne 2 ]]; then
	echo 'number of arguments should be 2'
	exit 1
fi

if [[ ! -e /dev/friend_loader || ! -e /dev/friend_loader2 ]]; then
	echo 'device file not exists'
	exit 1
fi

cat $1 | sudo dd of=/dev/friend_loader
cat $2 | sudo dd of=/dev/friend_loader2
