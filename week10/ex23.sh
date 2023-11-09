#!/bin/bash

mkdir -p ex23/

./gen.sh 10 ex23/ex1.txt

ln ex23/ex1.txt ex23/ex11.txt
ln ex23/ex1.txt ex23/ex12.txt

# they all have the same content as they all point to the same data
cat ex23/ex1.txt
echo ""
cat ex23/ex11.txt
echo ""
cat ex23/ex12.txt

# they are same inode numbers as they all point to the same data
stat -c "%i - %n" ex23/* > ex23/output.txt

# disk usage is 4 KB
du -h ex23/ex1.txt

ln ex23/ex1.txt ex23/ex13.txt
mv ex23/ex13.txt /tmp/ex13.txt

# shows all links in current directory
find . -inum $(stat -c "%i" ex23/ex1.txt)
echo ""
# shows all links in root
# for example, fd in proc: /proc/349770/task/349776/fd/20
find / -inum $(stat -c "%i" ex23/ex1.txt) 2> /dev/null

# there are 5 hard links
stat -c "%h" ex23/ex1.txt

rm /tmp/ex13.txt
find . -inum $(stat -c "%i" ex23/ex1.txt) -exec rm {} \;

rm -rf ex23/
