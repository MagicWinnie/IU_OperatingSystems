if [ $# -eq 0 ]
    then
        echo "Uh-oh! One argument N (1 <= N <= 4) should be provided!"
        exit 1
fi

if [ -d "/tmp/ex1" ]
    then
    if [[ $(ls "/tmp/ex1" | wc -l) > 0 ]]
        then
            echo "There are files in /tmp/ex1/. Deleting them..."
            rm /tmp/ex1/*
    fi
    else
        mkdir /tmp/ex1
fi

gcc publisher.c -o publisher.out
gcc subscriber.c -o subscriber.out

gnome-terminal --title="PUBLISHER" -- bash -c "./publisher.out $1; exec bash" &

echo "Waiting 2 seconds for publisher to run..."
sleep 2

for (( i = 1; i <= $1; i++ )) 
do 
    gnome-terminal --title="SUBSCRIBER $i" -- bash -c "./subscriber.out $i; exec bash" &
done
