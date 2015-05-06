#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "mpi.h"
 
int main (int argc, char* argv[]) {
    int rank, root, nproc, tag, size, arraysize, recsize, sendsize;
    int *nums;
    bool correct;
    MPI_Status status;

    size = 50000;
    root = 0;

    MPI_Init(&argc, &argv);
 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Comm_size(MPI_COMM_WORLD, &nproc);

    arraysize = size*(nproc);
    sendsize = size*rank;
    nums = (int *) malloc(sizeof(int)*arraysize);
    tag = 1;

    for (int sender=0; sender < nproc; sender++) {
        if(rank==sender) {
            for (int receiver = 0; receiver < nproc; receiver++) {
                if(receiver!=rank) {
                    for (int filler = 0; filler < sendsize; filler++) {
                        nums[filler] = (rank + receiver) * (filler + 1);
                    }
                    MPI_Send(nums, sendsize, MPI_INT, receiver, tag, MPI_COMM_WORLD);
                }
            }
            free(nums);
        } else {
            recsize = size*sender;
            //nums = (int *) malloc(sizeof(int)*recsize);
            MPI_Recv(nums, recsize, MPI_INT, sender, tag, MPI_COMM_WORLD, &status);
            correct = true;
            for (int filler = 0; filler < recsize; filler++) {
                if (nums[filler] != (rank + sender) * (filler + 1)) {
                    printf("%d got incorrect data from %d\n", rank, sender);
                    correct = false;
                    break;
                } 
            }
            if (correct) {
                printf("%d got correct data from %d\n", rank, sender);
            }
            //free(nums);
        }
    }

    free(nums);

    MPI_Finalize();

    return 0;
}
