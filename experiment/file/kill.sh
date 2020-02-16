#!/bin/bash

cd $(dirname $0)

while read line
do
	kill $line
done < pid.txt

# clear contents of pid.txt
> pid.txt
