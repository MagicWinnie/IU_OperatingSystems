gcc -pthread ex4.c -o ex4.out

rm -f ex4_res.txt

declare -i n=10000000
for m in 1 2 4 10 100
    do
        echo "======Running with arguments $n $m======" >> ex4_res.txt
        ( time ./ex4.out $n $m ) >> ex4_res.txt 2>> ex4_res.txt
done
