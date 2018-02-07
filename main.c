#include<mpi.h>
#include<stdio.h>
#include<stdlib.h> // rand(), srand()
#include<time.h>

#define RANKS 4
#define ROWS 10000
#define COLS 10000
#define CHUNKSIZE ROWS / RANKS

//int rank0();        // status
//int rankchild();    // status
int[] sum_chunk(int[][]);

int main(int argc, const char* argv) {
    
    srand(time(NULL)); // seed clock for rand()

    int size;    // needed for sending # of processes to MPI
    int **table; // deprecated
    int myrank;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int rank = myrank * CHUNKSIZE;

    int table[ROWS][COLS]; // randomly generated matrix decided by ROWS and COLS
    int chunk[CHUNKSIZE][COLS];

    // rank 0 generates matrix data and sums first chunk
    if (myrank == 0) {
        for (int i = 0; i < COLS; i++) {
            for (int j = 0; j < ROWS; j++) {
                table[i][j] = (int)rand();
            }
        }
        for (int i = 0; i < CHUNKSIZE; i++) {
            chunk[i][] = table[i][]
        }
        // mpisend 1 table[chunk1]
        // mpisend 2 table[chunk2]
        // mpisend 3 table[chunk3]
        int sums0[CHUNKSIZE] = sum_chunk(chunk);
        // sums1 mpirecv 1 
        // sums2 mpirecv 2 
        // sums3 mpirecv 3 
        int sums[ROWS];
        // sums = array_cat(sums0,sums1,sums2,sums3);
        // then we can open a fd for a new file and put them there
        int results = open("results.txt", O_WRONLY, O_CREAT);
        if (results < 1) { return results; }
        int fd_status;
        for ( int i = 0; i < ROWS; i++ ){
            fd_status = fprintf(results,"%d\n", sums[i]);
            if (fd_status < 1) { return fdstatus; }
        }
        fdstatus = close(results);
        if (fd_status < 1) { return fdstatus; }
        

    } else {
        // chunk = mpireceive myrank 
        int sums[CHUNKSIZE] = sum_chunk(chunk);
        //mpisend 0 sums[]
    }

    return 0;
}
/**
 * Deprecated for the moment
 */
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

/**
 * Row-wise summation of entire chunk, returns array of row sums
 */
int[] sum_chunk(int chunk[CHUNKSIZE][COLS]) {
    int sums[COLS] = {0}; 
    for (int i = 0; i < CHUNKSIZE; i++) {
        for (int j = 0; j < COLS; j++) {
            sums[i] += chunk[i][j];
        }
    }
    return sums;
}
