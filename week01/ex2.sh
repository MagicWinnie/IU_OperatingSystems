#!/bin/bash

# so history works in a script
HISTFILE=~/.bash_history
set -o history

# some commands
cd ..
cd week01
cat ex1.sh
pwd
ls -a

history | tail -n 6 > ex2.txt
