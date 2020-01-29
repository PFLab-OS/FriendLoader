#!/bin/bash

# errexit, nounset
set -eu

[[ $# -eq 2 ]]

export PATH=$PATH:~/FriendLoader/commands

for s in $(seq 0 6 36); do
	echo "file $s"
	file/run.sh $s
	for i in $(seq 1 100); do
		insmod.sh
		load.sh $1 $2
		sudo bash -c 'echo 3 > /proc/sys/vm/drop_caches'
		boot.sh
		sudo bash -c 'echo 3 > /proc/sys/vm/drop_caches'
		sudo cat /sys/module/friend_loader/dump \
			>> res0126/friendfileparallel$s.txt
		dump.sh
		rmmod.sh
		sleep 1
	done
	spin/kill.sh
done

