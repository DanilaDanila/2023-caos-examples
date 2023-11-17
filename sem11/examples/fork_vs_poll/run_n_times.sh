#!/bin/bash

if ! $(stat dos 1>/dev/null 2>/dev/null)
then
    echo 'run `make` first'
    exit 0
fi



N_THREADS=$1

# run_n_threads () {
#     n_threads=$1
#     cmd=$2
# 
#     if (($n_threads == 0))
#     then
#         return
#     fi
# 
#     ./$cmd &
#     run_n_threads $((n_threads - 1)) $cmd
# }

for _ in $(seq $N_THREADS)
{
    ./dos &
}

# run_n_threads $N_THREADS dos



for _ in $(seq $N_THREADS)
{
    wait
}

