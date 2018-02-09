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
int* sum_chunk(int**);

int main(int argc, char** argv) {
    srand(time(NULL)); // seed clock for rand()

    int size;    // needed for sending # of processes to MPI
    //int **table; // deprecated
    int myrank;
    //printf("here\n");
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    int rank = myrank * CHUNKSIZE;
    //printf("here\n");

    //int table[ROWS][COLS]; // randomly generated matrix decided by ROWS and COLS
    //int chunk [CHUNKSIZE][COLS];
    //int chunk0[CHUNKSIZE][COLS];
    //int chunk1[CHUNKSIZE][COLS];
    //int chunk2[CHUNKSIZE][COLS];
    //int chunk3[CHUNKSIZE][COLS];
    
    // Allocate row pointers + rows
    int **table  = (int**)malloc(ROWS*sizeof *table+(ROWS*(COLS*sizeof **table)));
    int **chunk0 = (int**)malloc(CHUNKSIZE*sizeof *chunk0 +(CHUNKSIZE * (COLS * sizeof **chunk0)));
    int **chunk1 = (int**)malloc(CHUNKSIZE*sizeof *chunk1 +(CHUNKSIZE * (COLS * sizeof **chunk1)));
    int **chunk2 = (int**)malloc(CHUNKSIZE*sizeof *chunk2 +(CHUNKSIZE * (COLS * sizeof **chunk2)));
    int **chunk3 = (int**)malloc(CHUNKSIZE*sizeof *chunk3 +(CHUNKSIZE * (COLS * sizeof **chunk3)));
    
    // initialize row-wise pointers. We're going to see this a few more times.
    int *rowptr_table  = (int*)table + ROWS;
    int *rowptr_chunk0 = (int*)chunk0 + CHUNKSIZE;
    int *rowptr_chunk1 = (int*)chunk1 + CHUNKSIZE;
    int *rowptr_chunk2 = (int*)chunk2 + CHUNKSIZE;
    int *rowptr_chunk3 = (int*)chunk3 + CHUNKSIZE;
    for (int i = 0; i < ROWS; i++) {
        table[i] = rowptr_table + i * COLS;
    }
    for (int i = 0; i < CHUNKSIZE; i++) {
        chunk0[i] = rowptr_chunk0 + i * CHUNKSIZE;
        chunk1[i] = rowptr_chunk1 + i * CHUNKSIZE;
        chunk2[i] = rowptr_chunk2 + i * CHUNKSIZE;
        chunk3[i] = rowptr_chunk3 + i * CHUNKSIZE;
    }
    // finish initialize row-wise pointers, for now.

    // rank 0 generates matrix data and sums first chunk
    if (myrank == 0) {
        
        printf("%d here",myrank);
        // random matrix data
        for (int i = 0; i < COLS; i++) {
            for (int j = 0; j < ROWS; j++) {
                table[i][j] = (int)rand();
            }
        }
        
        // split table into chunks
        for (int i = 0; i < CHUNKSIZE; i++) {
            for ( int j = 0; j < COLS; j++) {
                chunk0[i][j] = table[i*1-1][j];
                chunk1[i][j] = table[i*2-1][j];
                chunk2[i][j] = table[i*3-1][j];
                chunk3[i][j] = table[i*4-1][j];
            }
        }
        printf("here");

        MPI_Comm    comm1,comm2,comm3;
        MPI_Request request1,request2,request3;
        MPI_Status  status1,status2,status3;
        //
        // The Sends and receives need unique tag number
        //
        MPI_Isend(&chunk1,sizeof(chunk1),MPI_INT, 1, 1, comm1, &request1);
        MPI_Isend(&chunk2,sizeof(chunk2),MPI_INT, 2, 2, comm2, &request2);
        MPI_Isend(&chunk3,sizeof(chunk3),MPI_INT, 3, 3, comm3, &request3);

        int* sums0 = (int*)calloc(CHUNKSIZE,sizeof(int));
        int* sums1 = (int*)calloc(CHUNKSIZE,sizeof(int));
        int* sums2 = (int*)calloc(CHUNKSIZE,sizeof(int));
        int* sums3 = (int*)calloc(CHUNKSIZE,sizeof(int));

        // rank0 pulls its weight
        sums0 = sum_chunk(chunk0);

        MPI_Wait(&request1,&status1);
        MPI_Wait(&request2,&status2);
        MPI_Wait(&request3,&status3);

        MPI_Irecv(sums1, CHUNKSIZE*COLS, MPI_INT, 4, 4, comm1, &request1);
        MPI_Irecv(sums1, CHUNKSIZE*COLS, MPI_INT, 5, 5, comm2, &request2);
        MPI_Irecv(sums1, CHUNKSIZE*COLS, MPI_INT, 6, 6, comm3, &request3);

        MPI_Wait(&request1,&status1);
        MPI_Wait(&request2,&status2);
        MPI_Wait(&request3,&status3);

        // The following results print has some incorrently assumed maco definitions
        //int results = open("results.txt", O_WRONLY, O_CREAT);
        //if (results < 1) { return results; }
        //int fd_status;
        //for ( int i = 0; i < ROWS; i++ ){
        //    fd_status = fprintf(results,"%d\n", sums[i]);
        //    if (fd_status < 1) { return fdstatus; }
        //}
        //fdstatus = close(results);
        //if (fd_status < 1) { return fdstatus; }
        

    } else {

        printf("%d here",myrank);
        MPI_Comm comm;
        MPI_Request request;
        MPI_Status status;

        int** chunk = (int**)malloc(CHUNKSIZE*sizeof *chunk +(CHUNKSIZE * (COLS * sizeof **chunk)));
        int* rowptr_chunk = (int*)chunk + CHUNKSIZE;
        for (int i = 0; i < CHUNKSIZE; i++) {
            chunk[i] = rowptr_chunk + i * CHUNKSIZE;
        }

        int* sums = (int*)calloc(CHUNKSIZE,sizeof(int));

        MPI_Irecv(&chunk,sizeof(chunk),MPI_INT, myrank, myrank, comm, &request);


        MPI_Wait(&request,&status);
        sums = sum_chunk(chunk);
        MPI_Isend(&chunk,CHUNKSIZE*COLS,MPI_INT,myrank*2,myrank*2,comm,&request);
        MPI_Wait(&request,&status);
    }

    return MPI_Finalize();
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
int* sum_chunk(int** chunk) {

    int* sums = (int*)malloc(COLS* sizeof(int));

    for (int i = 0; i < CHUNKSIZE; i++) {
        for (int j = 0; j < COLS; j++) {
            sums[i] += chunk[i][j];
        }
    }
    return sums;
}
