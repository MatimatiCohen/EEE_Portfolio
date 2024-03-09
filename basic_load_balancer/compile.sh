#! /usr/bin/env bash

gcc -c lb.c
gcc -c utils.c
gcc -c lb_tools.c
gcc lb.o utils.o lb_tools.o -o lb
