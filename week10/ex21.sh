#!/bin/bash

# in /dev/ there are character files, block files, symbolic links, and directories
file /dev/*
# in /etc/ there are text files, symbolic links, and directories
file /etc/*

# there are 163 directories in /etc
ls -l /etc | grep ^d | wc -l

# file type of english.c is just C source, ASCII text
file ex21/english.c
gcc ex21/english.c -o ex21/english.out
# file type of english.out is now ELF 64-bit LSB shared object, x86-64, version 1 (SYSV), dynamically linked, interpreter /lib64/ld-linux-x86-64.so.2, BuildID[sha1]=1709f41eac79ef025713a8a7d22b91de15d6dd6f, for GNU/Linux 3.2.0, not stripped
# basically, it is now an executable rather than a text file
file ex21/english.out

# file type of nonenglish.c is now C source, UTF-8 Unicode text
# this happened as we are using non ASCII symbols in the source code
file ex21/nonenglish.c
gcc ex21/nonenglish.c -o ex21/nonenglish.out
# file type of nonenglish.out is now ELF 64-bit LSB shared object, x86-64, version 1 (SYSV), dynamically linked, interpreter /lib64/ld-linux-x86-64.so.2, BuildID[sha1]=cd6d0a663cc8938d3e4dade06557673da49731dd, for GNU/Linux 3.2.0, not stripped
# it is an executable, therefore, it does not store information about encoding
file ex21/nonenglish.out
