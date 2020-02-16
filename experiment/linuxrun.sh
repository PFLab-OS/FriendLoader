#!/bin/bash

# errexit, nounset
set -eu

if [[ $# -ne 1 ]]; then
	echo 'num of arg should be 1' >&2
	exit 1
fi

for s in $(seq 30 6 36); do
	echo "file $s"
	file/run.sh $s
	for n in $(seq 1 100); do
		sudo bash -c 'echo 3 > /proc/sys/vm/drop_caches'
		taskset --cpu-list 6,7 ./$1 >> res0126/linuxfileparallel$s.txt
	done
	file/kill.sh
done
