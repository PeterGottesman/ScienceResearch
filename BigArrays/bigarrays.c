#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mpi.h"
 
int main (int argc, char* argv[])
{
    int rank, root, nprocesses, source, target, tag;
    int size, arraysize, testsize, iterator;
    int correct, rcorrect;
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
 

    tag = sizeof(nums); //this is probably bad, don't do this

    for (source = 0; source < nprocesses; source++) {
        if(rank!=source) {
            testsize = source*size;
            test = (int*) malloc(sizeof(int)*testsize);
            for (iterator = 0; iterator < testsize; iterator++) {
                test[iterator] = (source + rank) * (iterator + 1); 
            }
            MPI_Irecv(&Rnums, testsize, MPI_INT, source, tag, MPI_COMM_WORLD, &request);
            MPI_Wait(&request, &status);
            if (rank != 0) {
                correct =  memcmp(Rnums, test, sizeof(Rnums));
                MPI_Send(&correct, 1, MPI_INT, root, tag, MPI_COMM_WORLD);
            } else { 
                MPI_Recv(&rcorrect, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &status);
            }
            free(test);
        }
    }

    for (target = 0; target < nprocesses; target++) {
        if(rank!=target) {
            MPI_Send(&nums, arraysize, MPI_INT, target, tag, MPI_COMM_WORLD);
        }
    } 
 
    free(nums);

    MPI_Finalize();

    return 0;
}
