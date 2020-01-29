# Friend Loader

## Requirements

* Run on HiKey960
* Add `mem=2G` to the Linux boot parameter

## Usage

* Fix `KERN_SRC` of `FriendLoader/Makefile`

* `make ready` will compile kernel module and a tool (toimg)

* `cd sample` and `make run` will do sample execution

* For the details, see each files
