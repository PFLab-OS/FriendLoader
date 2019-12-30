#/bin/bash

# errexit, nounset
set -eu

for i in `seq 1 400`
do
	./spin &
done
