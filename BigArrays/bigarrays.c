#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "mpi.h"
 
int main (int argc, char* argv[]) {
    int rank, root, nproc, tag, size, arraysize;
    int *nums;
    bool correct;

    size = 50000;
    root = 0;

    MPI_Init(&argc, &argv);
 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Comm_size(MPI_COMM_WORLD, &nproc);

    tag = 1;
    MPI_Request request[nproc*2];
    MPI_Status status[nproc*2];

    for (int sender=0; sender < nproc; sender++) {
        arraysize+=size*sender;
    }
    nums = (int *) malloc(sizeof(int)*arraysize);

    int place = 0;
    int numrequest = 0;
    for (int sender=0; sender < nproc; sender++) {
        place += size*sender;
        if(rank==sender) {
            for (int receiver = 0; receiver < nproc; receiver++) {
                if(receiver!=rank) {
                    for (int filler = 0; filler < size*sender; filler++) {
                        nums[filler] = (rank + receiver) * (filler + 1);
                    }
                    MPI_Isend(&nums[place], size*sender, MPI_INT, receiver, tag, MPI_COMM_WORLD, &request[numrequest++]);
                }
            }
        }
    }
    
    place = 0;
    for (int sender = 0; sender < nproc; sender++) {
        place += size*sender;
        if (rank!=sender) {
            MPI_Irecv(&nums[place], size*sender, MPI_INT, sender, tag, MPI_COMM_WORLD, &request[numrequest++]);
            correct = true;
            /*for (int filler = 0; filler < size*sender; filler++) {
                if (nums[filler] != (rank + sender) * (filler + 1)) {
                    printf("%d got incorrect data from %d\n", rank, sender);
                    correct = false;
                    break;
                } 
            }
            if (correct) {
                printf("%d got correct data from %d\n", rank, sender);
            }*/
        }

    }
    
    MPI_Waitall(nproc*2, request, status);

    

    free(nums);

    MPI_Finalize();

    return 0;
}
