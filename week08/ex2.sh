#!/bin/bash

if [ "$#" -ne 3 ]; then
    echo 'Usage: ./ex2.sh <number of pages> <number of frames> <"strings of memory accesses separated by spaces">'
    echo 'Example: ./ex2.sh 4 3 "R0 W1 R2">'
    exit 1
fi

gcc mmu.c -o mmu.out
gcc pager.c -o pager.out

rm -rf /tmp/ex2
mkdir /tmp/ex2

gnome-terminal --title="PAGER" -- bash -c "./pager.out $1 $2; exec bash" &

sleep 1

pid=$(cat /tmp/ex2/pager_pid)

./mmu.out $1 $3 $pid

rm -rf /tmp/ex2
