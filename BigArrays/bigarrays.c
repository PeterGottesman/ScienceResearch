#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mpi.h"
 
int main (int argc, char* argv[])
{
    int rank, root, nprocesses, source, target, tag;
    int size, arraysize, iterator;
    int correct;
    int *nums, *Rnums, *test;
    MPI_Status status;
    MPI_Request request;

 
    root = 0;
    size = 50000;
 
    MPI_Init(&argc, &argv);
 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    arraysize = rank*size;
    nums = (int*) malloc(sizeof(int)*arraysize);
 
    MPI_Comm_size(MPI_COMM_WORLD, &nprocesses);

    for (iterator = 0; iterator < arraysize; iterator++) {
        nums[iterator] = (rank + target) * (iterator + 1); 
    }
 
    tag = nums;

    for (source = 0; source < nprocesses; source++) {
        if(rank!=source) {
            for (iterator = 0; iterator < arraysize; iterator++) {
                test[iterator] = (source + rank) * (iterator + 1); 
            }
            MPI_IRECV(&Rnums, arraysize, MPI_INT, source, tag, MPI_COMM_WORLD, &request);
            MPI_WAIT(&request, &status);
            if (rank != 0) {
                MPI_SEND(memcmp(Rnums, test, sizeof(Rnums)), arraysize, MPI_INT, root, tag, MPI_COMM_WORLD);
            } else { 
                MPI_RECV(&correct, 1, MPI_INT, source, tag, MPI_COMM_WORLD);
            }
        }
    }

    for (target = 0; target < nprocesses; target++) {
        if(rank!=target) {
            
            MPI_SEND(&nums, arraysize, MPI_INT, target, tag, MPI_COMM_WORLD);
        }
    } 
 
    MPI_Finalize();

    return 0;
}
