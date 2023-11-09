#!/bin/bash

echo "" > ex25/ex5.txt

chmod a-w ex25/ex5.txt

chmod uo+rwx ex25/ex5.txt

chmod g=u ex25/ex5.txt

# 660 means that owner and group users can read/write to this file,
# while others do not have any access
# -rw-rw----

# 775 means that owner and group users can do everything to this file,
# while others can read and execute
# -rwxrwxr-x

# 777 means that everybody can do everything to this file
# -rwxrwxrwx

rm ex25/ex5.txt
