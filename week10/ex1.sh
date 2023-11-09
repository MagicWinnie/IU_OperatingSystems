#!/bin/bash

gcc monitor.c -o monitor.out
gcc ex1.c -o ex1.out

gnome-terminal --title="MONITOR" -- bash -c "./monitor.out $(pwd); exec bash" &
sleep 3

mkdir test_folder
./ex1.out test_folder
rm -r test_folder

./ex1_test.sh
