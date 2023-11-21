#!/bin/bash

disk_name=$(cat input.txt | head -1)
echo "[INFO] Using disk $disk_name"

if [ ! -f $disk_name ]; then
    echo "[INFO] Disk $disk_name does not exist, creating it"
    gcc create_fs.c -o create_fs.out && ./create_fs.out $disk_name
    rm create_fs.out
else
    echo "[INFO] Disk $disk_name exists, not creating it"
fi

gcc ex2.c -o ex2.out -Wall && ./ex2.out
