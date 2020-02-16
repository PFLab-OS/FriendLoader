#!/bin/bash

# errexit, nounset
set -eu

cd $(dirname $0)

if [[ $# -ne 1 ]]; then
	echo 'num of args should be 1' >&2
	exit 1
fi

for i in $(seq 1 $1)
do
	./spin.sh &
	echo $! >> pid.txt
done
