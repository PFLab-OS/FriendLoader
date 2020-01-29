#!/bin/bash

# Usage:
# boot
# will boot the friend tasks if they have been already loaded.

echo 1 | sudo tee /sys/module/friend_loader/boot
