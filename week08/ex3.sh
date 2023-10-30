#!/bin/bash

gcc ex3.c -o ex3.out && (./ex3.out &)
vmstat 1 11
