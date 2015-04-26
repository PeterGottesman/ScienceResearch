#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mpi.h"
 
int main (int argc, char* argv[]) {
    int rank, root, nproc, tag, size, arraysize, recsize;
    int *nums, *rnums, *rectest;
    MPI_Status status;

    size = 50000;
    root = 0;

    MPI_Init(&argc, &argv);
 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    arraysize = size*rank;
    nums = (int *) malloc(sizeof(int)*arraysize);

    MPI_Comm_size(MPI_COMM_WORLD, &nproc);

    tag = 1;

    for (int sender=0; sender < nproc; sender++) {
        if(rank==sender) {
            for (int receiver = 0; receiver < nproc; receiver++) {
                if(receiver!=rank) {
                    for (int filler = 0; filler < arraysize; filler++) {
                        nums[filler] = (rank + receiver) * (filler + 1);
                    }
                    MPI_Send(nums, arraysize, MPI_INT, receiver, tag, MPI_COMM_WORLD);
                }
            }
        } else {
            recsize = size*sender;
            rectest = (int *) malloc(sizeof(int)*recsize);
            for (int filler = 0; filler < recsize; filler++) {
                rectest[filler] = (rank + sender) * (filler + 1);
            }
            rnums = (int *) malloc(sizeof(int)*recsize);
            MPI_Recv(rnums, recsize, MPI_INT, sender, tag, MPI_COMM_WORLD, &status);
            if(memcmp(rectest, rnums, recsize)==0) {
                printf("rank %d got correct data from %d \n", rank, sender);
            } else {
                printf("rank %d got incorrect data from %d \n", rank, sender);
            }
            free(rnums);
            free(rectest);
        }
    }

    free(nums);

    MPI_Finalize();

    return 0;
}
