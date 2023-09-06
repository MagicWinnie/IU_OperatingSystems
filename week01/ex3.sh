#!/bin/bash

date
sleep 3

mkdir root_folder

date
sleep 3

ls -tr / > root_folder/root.txt

date
sleep 3

mkdir home_folder

date
sleep 3

ls -tr ~ > home_folder/home.txt

echo "-----Content of root.txt-----"
cat root_folder/root.txt

echo "-----Content of home.txt-----"
cat home_folder/home.txt

echo "-----Items of root_folder-----"
ls root_folder

echo "-----Items of home_folder-----"
ls home_folder
