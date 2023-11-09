#!/bin/bash

watched_dir="watched_dir"

gcc monitor.c -o monitor.out
gcc ex1.c -o ex1.out

rm -rf $watched_dir
mkdir -p $watched_dir

gnome-terminal --title="MONITOR" -- bash -c "./monitor.out $watched_dir; exec bash" &
sleep 3

./ex1.out $watched_dir

cd $watched_dir
../ex1_test.sh
cd ..
