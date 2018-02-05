#include<mpi.h>
#include<stdio.h>
#include<stdlib.h> // rand(), srand()
#include<time.h>

#define RANKS 4
#define ROWSIZE 10000
#define COLSIZE 10000
#define CHUNKSIZE ROWSIZE / RANKS

//int rank0();        // status
//int rankchild();    // status
int[] sum_chunk(int[][]);

int main(int argc, const char* argv) {
    
    srand(time(NULL)); // seed clock for rand()

    int size;
    int **table;
    int myrank;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int rank = myrank * CHUNKSIZE;

    int table[ROWSIZE][COLSIZE];

    int chunk[CHUNKSIZE][COLSIZE];

    if (myrank == 0) {
        for (int i = 0; i < ROWSIZE; i++) {
            for (int j = 0; j < ROWSIZE; j++) {
                table[i][j] = (int)rand();
            }
        }
        // chunk[][] = table[chunk0][]
        // mpisend 1 table[chunk1]
        // mpisend 2 table[chunk2]
        // mpisend 3 table[chunk3]
        int sums0[CHUNKSIZE] = sum_chunk(chunk);
        // sums1 mpirecv 1 
        // sums2 mpirecv 2 
        // sums3 mpirecv 3 
        // sums = array_cat(sums0,sums1,sums2,sums3);
        // then we can open a fd for a new file and put them there
    } else {
        // chunk = mpireceive myrank 
        int sums[CHUNKSIZE] = sum_chunk(chunk);
        //mpisend 0 chunk[]
    }

    return 0;
}
//int rank0() {
//    //generate table
//
//    //send data
//    MPI_Isend(,ROWRANKCOUNT,
//    //compute
//    //sums = sum_array(subtable[0-2499]);)
//    //for i in range (RANKS)
//    //    MPI_Irecv(&sums[RANKS-1 - RANKS*2500-1],rank...)
//    //
//
//    return 0;
//}
//int rankchild() {
//    MPI_Irecv()
//    return 0;
//}
//

int[] sum_chunk(int chunk[CHUNKSIZE][COLSIZE]) {
    int sums[COLSIZE] = 0
    for (int i = 0; i < CHUNKSIZE; i++) {
        sums[i] = 0;
        for (int j = 0; j < CHUNKSIZE; j++) {
            sums[i] += chunk[i][j];
        }
    }
    return sums;

}
