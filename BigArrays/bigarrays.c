#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mpi.h"
 
int main (int argc, char* argv[])
{
    int rank, root, nprocesses, source, target, tag;
    int size, arraysize, recvsize, iterator;
    int correct, rcorrect;
    int *nums, *Rnums, *test;
    MPI_Status status;
    MPI_Request request, request2;

    root = 0;
    size = 50000;
    
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank)

    arraysize = rank*size;
    nums = (int*) malloc(sizeof(int)*arraysize);
 
    MPI_Comm_size(MPI_COMM_WORLD, &nprocesses);

 

    tag = 10;

    for (target = 0; target < nprocesses; target++) {
        if(rank!=target) {
            for (iterator = 0; iterator < arraysize; iterator++) {
                nums[iterator] = (rank + target) * (iterator + 1); 
            }
            printf("presend %d to %d\n", rank, target);
            MPI_Isend(nums, arraysize, MPI_INT, target, tag, MPI_COMM_WORLD, &request2);
            printf("sent %d to %d\n", rank, target);
        }
    } 
 
    for (source = 0; source < nprocesses; source++) {
        if(rank!=source) {
            recvsize = source*size;
            test = (int*) malloc(sizeof(int)*recvsize);
            for (iterator = 0; iterator < recvsize; iterator++) {
                test[iterator] = (source + rank) * (iterator + 1);
            }
            Rnums = (int*) malloc(sizeof(int)*recvsize);
            MPI_Wait(&request2, &status);
            MPI_Irecv(Rnums, recvsize, MPI_INT, source, tag, MPI_COMM_WORLD, &request);
            printf("wait %d from %d\n", rank, source);
            MPI_Wait(&request, &status);
            //if (rank != root) {
                printf("%d checking send of %d\n", rank, source);
                correct =  memcmp(Rnums, test, recvsize);
                if (correct != 0) {
                    printf("-------------rank %d got incorrect data from %d\n", rank, source);
                    //MPI_Abort(MPI_COMM_WORLD, 1);
                } else {
                    printf("rank %d got correct data from %d \n", rank, source);
                }
                //MPI_Send(&correct, 1, MPI_INT, root, tag, MPI_COMM_WORLD);
            //}
            free(Rnums);
            free(test);
        }
        if (rank==root && rank!=source) {  
            //MPI_Recv(&rcorrect, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &status);
        }
    }


    free(nums);

    MPI_Finalize();

    return 0;
}
