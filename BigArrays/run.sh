#!/bin/sh
rm -f out.log
mpicc -g -o bigarrays bigarrays.c -std=c11
echo "How many processes should be run?"
read nproc
mpirun -np $nproc bigarrays > log.out
echo "What patterns should be searched for?"
read -a searches
for search in "${searches[@]}"
do
    echo "===== $search ====="
    grep "$search" log.out | sort 
done
