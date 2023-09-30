gcc ex3.c -o ex3.out

echo "Running for n = 3"
./ex3.out 3 &

for i in {0..3}
do
    echo "Running pstree for $i time..."
    pstree -p -a | grep ex3.out
    sleep 5
done


echo -e "\n\nRunning for n = 5"
./ex3.out 5 &

for i in {0..5}
do
    echo "Running pstree for $i time..."
    pstree -p -a | grep ex3.out
    sleep 5
done
