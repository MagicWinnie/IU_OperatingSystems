#!/bin/bash

# inode is 3943372
# number of blocks is 8
# block size is 4096
# total size is 84 bytes
# permissions are -rw-rw-r--
stat ex22/ex1.c

# they have the same number of links because they are different file
# we did not create any links
cp ex22/ex1.c ex22/ex2.c
stat ex22/ex2.c

# it shows how many directories contain a name number mapping for that i-node
stat -c "%h - %n" /etc/* | grep ^3
