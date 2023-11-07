#!/bin/bash

if [ "$#" -ne 4 ]; then
    echo 'Usage: ./ex1.sh <number of pages> <number of frames> <page replacement algorithm> <"strings of memory accesses separated by spaces">'
    echo 'Example: ./ex1.sh 4 3 random "R0 W1 R2">'
    exit 1
fi

gcc mmu.c -o mmu.out
gcc pager.c -o pager.out

rm -rf /tmp/ex1
mkdir /tmp/ex1

gnome-terminal --title="PAGER" -- bash -c "./pager.out $1 $2 $3; exec bash" &

sleep 1

pid=$(cat /tmp/ex1/pager_pid)

./mmu.out $1 $4 $pid

rm -rf /tmp/ex1
