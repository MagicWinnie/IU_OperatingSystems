gcc -pthread ex3.c -o ex3.out

rm -f ex3_res.txt

declare -i n=10000000
for m in 1 2 4 10 100
    do
        echo "======Running with arguments $n $m======" >> ex3_res.txt
        ( time ./ex3.out $n $m ) >> ex3_res.txt 2>> ex3_res.txt
done
