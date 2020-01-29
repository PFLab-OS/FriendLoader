#!/bin/bash

# errexit, nounset
set -eu

NAME=sample
export PATH=$PATH:~/FriendLoader/commands

insmod.sh
load.sh ${NAME}1.img ${NAME}2.img
boot.sh
dump.sh
rmmod.sh
