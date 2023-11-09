#!/bin/bash

rm -rf ex24/tmp/
# symbolic link can be created to a non existing folder
ln -sd $(realpath ex24/tmp/) $(realpath ex24/tmp1/)

# only a symbolic link exists
echo "==============ls -li ex24/================"
ls -li ex24/

mkdir ex24/tmp/

# now a folder `tmp` and a symbolic link exist
echo "==============ls -li ex24/================"
ls -li ex24/

./gen.sh 10 ex24/tmp/ex1.txt
# as we linked `tmp` and `tmp1`, `ex1.txt` also exists in `tmp1`
echo "============ls -li ex24/tmp1/============="
ls -l ex24/tmp1/

ln -sd $(realpath ex24/tmp/) $(realpath ex24/tmp1/tmp2/)

./gen.sh 10 ex24/tmp1/tmp2/ex1.txt

# we have created a recursion, we can access as many `tmp2`: ex24/tmp1/(tmp2)*
echo "===============tree ex24/================="
tree ex24/
echo "======ls -l ex24/tmp1/tmp2/tmp2/tmp2======"
ls -l ex24/tmp1/tmp2/tmp2/tmp2

# symbolic link is still present
rm -r ex24/tmp/
echo "==============ls -li ex24/================"
ls -li ex24/

rm -r ex24/
mkdir ex24/
