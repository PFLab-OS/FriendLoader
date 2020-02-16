#!/bin/bash

# Usage:
# dump
# will dump the contents of friend_loader_buf through kernel message

sudo bash -c 'echo 3 > /proc/sys/vm/drop_caches'
sudo bash -c 'echo 1 > /sys/module/friend_loader/dump'
