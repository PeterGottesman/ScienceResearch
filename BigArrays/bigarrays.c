#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "mpi.h"
 
int main (int argc, char* argv[]) {
    int rank, root, nproc, tag, size, arraysize;
    int *nums, *rnums;
    bool correct;

    size = 50000;
    root = 0;

    MPI_Init(&argc, &argv);
 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Comm_size(MPI_COMM_WORLD, &nproc);

    tag = 1;
    MPI_Request request[(nproc-1)*2];
    MPI_Status status[(nproc-1)*2];

    arraysize = 0;
    for (int sender=0; sender <= nproc; sender++) {
        arraysize+=size*sender;
    }
    rnums = (int *) malloc(sizeof(int)*arraysize);
    if (rnums==NULL) {
        fprintf(stderr, "rnums did not malloc correctly in %d\n", rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    nums = (int *) malloc(sizeof(int)*size*rank*(nproc-1));
    if (nums==NULL) {
        fprintf(stderr, "nums did not malloc correctly in %d\n", rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int place = 0;
    int numrequest = 0;
    printf("sender %d is filling starting with position %d\n", rank, place);
    for (int receiver = 0; receiver < nproc; receiver++) {
        if(receiver!=rank) {
            printf("sender %d is sending to receiver %d starting at position %d\n", rank, receiver, place);
            for (int filler = 0; filler < size*rank; filler++) {
                nums[filler+place] = (rank + receiver) * (filler + 1);
                //nums[filler+place]=rank;
            }
            printf("%d sent %d to %d\n", rank, nums[place], receiver);
            MPI_Isend(&nums[place], size*rank, MPI_INT, receiver, tag, MPI_COMM_WORLD, &request[numrequest++]);
            place += size*rank;
        }
    }
    
    place = 0;
    for (int sender = 0; sender < nproc; sender++) {
        if (rank!=sender) {
            MPI_Irecv(&rnums[place], size*sender, MPI_INT, sender, tag, MPI_COMM_WORLD, &request[numrequest++]);
            place += size*sender;
        }
    }
    
    MPI_Waitall((nproc-1)*2, request, status);
    free(nums);
    
    place = 0;
    for (int sender = 0; sender < nproc; sender++) {
        if (rank!=sender) {
            printf("%d got %d from %d\n", rank, rnums[place], sender);
            correct = true;
            for (int filler = 0; filler < size*sender; filler++) {
                if (rnums[filler+place] != (rank + sender) * (filler + 1)) {
                    printf("%d got incorrect data from %d, expecting %d got %d at %d, or %d including place\n", rank, sender, (rank+sender)*(filler+1), rnums[filler+place], filler, filler+place);
                    correct = false;
                    break;
                } 
            }
            if (correct) {
                printf("%d got correct data from %d\n", rank, sender);
            }
            place += size*sender;
        }
    }

    free(rnums);

    MPI_Finalize();

    return 0;
}
