#!/bin/bash

# build
if ! gcc -O0 main.c -o test
then
    exit 1
fi


# L0 dump
# heredoc
# <<ABC
# ...
# ABC
r2 test 2>/dev/null 1>l0.s <<EOF
aaaa
s sym.l0_do_nothing
pdf
EOF


# L1 dump
r2 test 2>/dev/null 1>l1.s <<EOF
aaaa
s sym.l1_do_nothing
pdf
EOF


# L2 dump
r2 test 2>/dev/null 1>l2.s <<EOF
aaaa
s sym.l2_do_nothing
pdf
EOF


# MMAP dump
r2 test 2>/dev/null 1>mmap.s <<EOF
aaaa
s sym.mmap_do_nothing
pdf
EOF


# run
./test

