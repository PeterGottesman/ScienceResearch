#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <getopt.h>
#include "mpi.h"
 
int main (int argc, char* argv[]) {
    double start, end;
    int rank, root, nproc, tag, size, arraysize;
    int *nums, *rnums;
    bool correct;
    
    static int verify;
    static int plot;
    static int message_size;
    int getoptc;
    
    while (1) {
        static struct option long_options[] = {
            {"plot", no_argument, &plot, 1},
            {"verify", no_argument, &verify, 1},
            {"message_size", required_argument, 0, 'm'}
        };
        
        int options_index = 0;

        getoptc = getopt_long (argc, argv, "", long_options, &options_index);
        
        if (getoptc == -1) break;

        switch (getoptc) {
            case 'm':
                if (atoi(optarg) > 0) size = atoi(optarg);
                else size = 50000;
                break;

            case '?':
                break;

            default:
                break;
        } 
    }

    root = 0;

    MPI_Init(&argc, &argv);

    start = MPI_Wtime();

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
    if (!plot) printf("sender %d is filling starting with position %d\n", rank, place);
    for (int receiver = 0; receiver < nproc; receiver++) {
        if(receiver!=rank && rank != 0) {
            if (!plot) printf("sender %d is sending to receiver %d starting at position %d\n", rank, receiver, place);
            for (int filler = 0; filler < size*rank; filler++) {
                nums[filler+place] = (rank + receiver) * (filler + 1);
            }
            if (!plot) printf("%d sent %d to %d\n", rank, nums[place], receiver);
            MPI_Isend(&nums[place], size*rank, MPI_INT, receiver, tag, MPI_COMM_WORLD, &request[numrequest++]);
            place += size*rank;
        }
    }
    
    place = 0;
    for (int sender = 1; sender < nproc; sender++) {
        if (rank!=sender) {
            MPI_Irecv(&rnums[place], size*sender, MPI_INT, sender, tag, MPI_COMM_WORLD, &request[numrequest++]);
            place += size*sender;
        }
    }
    
    MPI_Waitall(numrequest, request, status);
    free(nums);
    
    if (verify) {
        place = 0;
        for (int sender = 0; sender < nproc; sender++) {
            if (rank!=sender) {
                if (!plot) printf("%d got %d from %d\n", rank, rnums[place], sender);
                correct = true;
                for (int filler = 0; filler < size*sender; filler++) {
                    if (rnums[filler+place] != (rank + sender) * (filler + 1)) {
                        printf("%d got incorrect data from %d, expecting %d got %d at %d, or %d including place\n", rank, sender, (rank+sender)*(filler+1), rnums[filler+place], filler, filler+place);
                        correct = false;
                        break;
                    } 
                }
                if (correct) {
                    if (!plot) printf("%d got correct data from %d\n", rank, sender);
                }
                place += size*sender;
            }
        }
    } 

    free(rnums);

    end = MPI_Wtime();

    if (!plot) {
        printf("%d elapsed time: %f(ms)\n", rank, (end-start)*1000.0);
    } else {
        printf("%d %f\n", rank, (end-start)*1000.0);
    }
    MPI_Finalize();

    return 0;
}
