if [ $# -eq 0 ]
    then
        echo "Uh-oh! Path to the input file should be provided!"
        exit 1
fi

gcc worker.c -o worker.out
gcc scheduler_rr.c -o scheduler_rr.out

./scheduler_rr.out $1
