#!/bin/bash

if [ $# -eq 0 ]
    then
        echo "Uh-oh! Path to the input file should be provided!"
        exit 1
fi

gcc worker.c -o worker.out
gcc scheduler_sjf.c -o scheduler_sjf.out

./scheduler_sjf.out $1
