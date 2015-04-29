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

    MPI_Comm_size(MPI_COMM_WORLD, &nproc);

    tag = 1;
    MPI_Request request[nproc*2];

    for (int sender = 0; sender < nproc; sender++) {
        arraysize += size * sender;
    }
    nums = (int *) malloc(sizeof(int) * arraysize);

    int place = 0;
    for (int sender = 0; sender < nproc; sender++) {
        place += size * sender;
        if(sender == rank) {
            for (int receiver = 0; receiver < nproc; receiver++) {
                if(receiver!=rank) {
                    MPI_Isend(nums[place], sender * size, MPI_INT, receiver, tag, MPI_COMM_WORLD, &request[sender+(nproc-1)]);
                }
            }
        } else {
            recsize = size*sender;
            rectest = (int *) malloc(sizeof(int)*recsize)
            for (int filler = 0; filler < recsize; filler++) {
                rectest[filler] = (rank + sender) * (filler + 1);
            }
            rnums = (int *) malloc(sizeof(int)*recsize);
            MPI_Irecv(&rnums[place], size * sender, MPI_INT, tag, sender, MPI_COMM_WORLD, &request[sender]);
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
