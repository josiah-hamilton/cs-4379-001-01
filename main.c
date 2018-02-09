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
void sum_chunk(int**,int*,int);

int main(int argc, char** argv) {
    srand(time(NULL)); // seed clock for rand()

    int size;    // needed for sending # of processes to MPI
    int myrank;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int rank = myrank * CHUNKSIZE;

    // rank 0 generates matrix data and sums first chunk
    if (myrank == 0) {

        // Allocate row pointers + rows
        int **table  = (int**) calloc(ROWS,sizeof(int *));
        int **chunk0 = (int**) calloc(CHUNKSIZE,sizeof(int *));
        int **chunk1 = (int**) calloc(CHUNKSIZE,sizeof(int *));
        int **chunk2 = (int**) calloc(CHUNKSIZE,sizeof(int *));
        int **chunk3 = (int**) calloc(CHUNKSIZE,sizeof(int *));

        for (int i = 0; i < ROWS; i++) {
            table[i] = (int *)calloc(COLS,sizeof(int));
        }
        for (int i = 0; i < CHUNKSIZE; i++) {
            chunk0[i] = (int *)calloc(COLS,sizeof(int));
            chunk1[i] = (int *)calloc(COLS,sizeof(int));
            chunk2[i] = (int *)calloc(COLS,sizeof(int));
            chunk3[i] = (int *)calloc(COLS,sizeof(int));
        }
        
        // random matrix data
        for (int i = 0; i < COLS; i++) {
            for (int j = 0; j < ROWS; j++) {
                table[i][j] = (int)rand();
            }
        }
        
        // split table into chunks
        for (int i = 0; i < CHUNKSIZE; i++) {
            for ( int j = 0; j < COLS; j++) {
                chunk0[i][j] = table[i+0][j];
                chunk1[i][j] = table[i+2500][j];
                chunk2[i][j] = table[i+5000][j];
                chunk3[i][j] = table[i+7500][j];
            }
        }

        MPI_Request request1,request2,request3;
        MPI_Status  status1,status2,status3;
        //
        // The Sends and receives need unique tag number
        //
        MPI_Isend(&chunk1,sizeof(int)*COLS*CHUNKSIZE+sizeof(int*)*CHUNKSIZE,MPI_INT, 1, 1, MPI_COMM_WORLD, &request1);
        MPI_Isend(&chunk2,sizeof(int)*COLS*CHUNKSIZE+sizeof(int*)*CHUNKSIZE,MPI_INT, 2, 2, MPI_COMM_WORLD, &request2);
        MPI_Isend(&chunk3,sizeof(int)*COLS*CHUNKSIZE+sizeof(int*)*CHUNKSIZE,MPI_INT, 3, 3, MPI_COMM_WORLD, &request3);
        //printf("%d: chunk size: %d\n",myrank,sizeof(int)*COLS*CHUNKSIZE+sizeof(int*)*CHUNKSIZE);

        int* sums0 = (int*)calloc(CHUNKSIZE,sizeof(int));
        int* sums1 = (int*)calloc(CHUNKSIZE,sizeof(int));
        int* sums2 = (int*)calloc(CHUNKSIZE,sizeof(int));
        int* sums3 = (int*)calloc(CHUNKSIZE,sizeof(int));

        // rank0 pulls its weight
        sum_chunk(chunk0,sums0,myrank);
        printf("%d here\n",myrank);

        MPI_Wait(&request1,&status1);
        MPI_Wait(&request2,&status2);
        MPI_Wait(&request3,&status3);

        MPI_Irecv(sums1,sizeof(int)*CHUNKSIZE, MPI_INT, 1, 4, MPI_COMM_WORLD, &request1);
        MPI_Irecv(sums1,sizeof(int)*CHUNKSIZE, MPI_INT, 2, 5, MPI_COMM_WORLD, &request2);
        MPI_Irecv(sums1,sizeof(int)*CHUNKSIZE, MPI_INT, 3, 6, MPI_COMM_WORLD, &request3);

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

        MPI_Request request;
        MPI_Status status;

        int **chunk = (int **)calloc(CHUNKSIZE,sizeof(int*));
        for (int i = 0; i < ROWS; i++) {
            chunk[i] = (int *)calloc(COLS,sizeof(int));
        }

        int* sums = (int*)calloc(CHUNKSIZE,sizeof(int));

        MPI_Irecv(&chunk,sizeof(int)*COLS*CHUNKSIZE+sizeof(int*)*CHUNKSIZE,MPI_INT, 0, myrank, MPI_COMM_WORLD, &request);
        MPI_Wait(&request,&status);

        //printf("%d: chunk size: %d\n",myrank,sizeof(int)*COLS*CHUNKSIZE+sizeof(int*)*CHUNKSIZE);
        sum_chunk(chunk,sums,myrank);
        printf("%d here\n",myrank);
        MPI_Isend(&chunk,sizeof(int)*CHUNKSIZE,MPI_INT,0,myrank*2,MPI_COMM_WORLD,&request);
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
 * Row-wise summation of entire chunk, 
 * takes as its inputs a 2D array of integers, chunk, 
 * and the pointer to a summation array, sums
 * (for debugging also takes the rank identifier)
 * Returns by referense the sums of each element of a given row.
 */
void sum_chunk(int **chunk, int *sums, int myrank) {
    sums[0]=0;
    //printf("%d\t%d\n",myrank,sums[0]);
    int i,j;
    for (i = 0; i < CHUNKSIZE; i++) {
        //printf("%d:\t%d\t%x\t%d\n",myrank,chunk[i][0],sums[i],i);
        for (j = 0; j < COLS; j++) {
            sums[i] += chunk[i][j];
        }
        //printf("%d: %d\n",myrank,i); 
    }
    //printf("%d sum done\n",myrank);
}
