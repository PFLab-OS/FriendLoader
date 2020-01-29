#!/bin/bash

# Usage:
# rmmod
# will remove friend_loader and delete device files

# errexit, nounset
set -eu

if lsmod | grep friend_loader; then
	sudo rmmod friend_loader
fi

if [[ -e /dev/friend_loader ]]; then
	sudo rm /dev/friend_loader
fi
if [[ -e /dev/friend_loader2 ]]; then
	sudo rm /dev/friend_loader2
fi
