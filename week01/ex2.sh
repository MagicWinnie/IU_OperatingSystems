#!/bin/bash

# so we can see history in noninteractive shells 
HISTFILE=~/.bash_history
set -o history

# some commands
cd ..
cd week01
cat ex1.sh
pwd
ls -a

history | tail -n 6 > ex2.txt
