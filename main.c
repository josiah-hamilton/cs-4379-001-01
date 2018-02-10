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
void sum_chunk(int*,int*,int,int);

int main(int argc, char** argv) {
    srand(time(NULL)); // seed clock for rand()
    int ranks=RANKS;
    int rows=ROWS;
    int cols=COLS;
    int chunkrows=rows / ranks;

    int size;    // needed for sending # of processes to MPI
    int myrank;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int rank = myrank * CHUNKSIZE;
    int chunkbufcount= cols*chunkrows;

    // rank 0 generates matrix data and sums first chunk
    if (myrank == 0) {

        MPI_Request request1,request2,request3,request4,request5,request6;
        MPI_Status  status1,status2,status3,status4,status5,status6;
        
        // Allocate row pointers + rows
        int *table  = (int*) calloc(rows * cols,sizeof(int *));
        
        // random matrix data
        for (int i = 0; i < cols; i++) {
            for (int j = 0; j < cols; j++) {
                *(table + cols*i + j) = (int)rand(); // i+j;
            }
        }

        // The Sends and receives need unique tag number
        
        MPI_Isend(table+1*cols*chunkrows,chunkbufcount,MPI_INT,1,1,MPI_COMM_WORLD,&request1);
        MPI_Isend(table+2*cols*chunkrows,chunkbufcount,MPI_INT,2,2,MPI_COMM_WORLD,&request2);
        MPI_Isend(table+3*cols*chunkrows,chunkbufcount,MPI_INT,3,3,MPI_COMM_WORLD,&request3);

        int* sums0 = (int*)calloc(CHUNKSIZE,sizeof(int));
        int* sums1 = (int*)calloc(CHUNKSIZE,sizeof(int));
        int* sums2 = (int*)calloc(CHUNKSIZE,sizeof(int));
        int* sums3 = (int*)calloc(CHUNKSIZE,sizeof(int));


        // rank0 pulls its weight
        sum_chunk(table,sums0,chunkbufcount,cols);

        MPI_Wait(&request1,&status1);
        MPI_Wait(&request2,&status2);
        MPI_Wait(&request3,&status3);

        MPI_Irecv(sums1,chunkrows,MPI_INT,1,4,MPI_COMM_WORLD,&request4);
        MPI_Irecv(sums2,chunkrows,MPI_INT,2,5,MPI_COMM_WORLD,&request5);
        MPI_Irecv(sums3,chunkrows,MPI_INT,3,6,MPI_COMM_WORLD,&request6);

        MPI_Wait(&request4,&status4);
        MPI_Wait(&request5,&status5);
        MPI_Wait(&request6,&status6);

        //int results = open("results.txt", "w");
        //if (results < 1) { return results; }
        //int fd_status;
        //for ( int i = 0; i < chunkrows; i++ ){
        //    fd_status = fprintf(results,"%d\n", sums0[i]);
        //    if (fd_status < 1) { return fd_status; }
        //}
        //for ( int i = 0; i < chunkrows; i++ ){
        //    fd_status = fprintf(results,"%d\n", sums1[i]);
        //    if (fd_status < 1) { return fd_status; }
        //}
        //for ( int i = 0; i < chunkrows; i++ ){
        //    fd_status = fprintf(results,"%d\n", sums2[i]);
        //    if (fd_status < 1) { return fd_status; }
        //}
        //for ( int i = 0; i < chunkrows; i++ ){
        //    fd_status = fprintf(results,"%d\n", sums3[i]);
        //    if (fd_status < 1) { return fd_status; }
        //}
        //fd_status = close(results);
        //if (fd_status < 1) { return fd_status; }
        
        free(table);
        free(sums0);
        free(sums1);
        free(sums2);
        free(sums3);

    } else {

        MPI_Request request,request2;
        MPI_Status status,status2;

        int *chunk = (int*) calloc(CHUNKSIZE * COLS,sizeof(int *));
        int *sums  = (int*) calloc(CHUNKSIZE,sizeof(int));

        MPI_Irecv(chunk,chunkbufcount,MPI_INT,0,myrank,MPI_COMM_WORLD,&request);
        MPI_Wait(&request,&status);

        sum_chunk(chunk,sums,chunkbufcount,cols);

        MPI_Isend(sums,chunkrows,MPI_INT,0,myrank+3,MPI_COMM_WORLD,&request2);
        MPI_Wait(&request2,&status2);

        free(chunk);
        free(sums);
    }

    //printf("%d: finish\n",myrank);
    return MPI_Finalize(); 
}

/**
 * Row-wise summation of entire chunk, 
 * takes as its inputs a 2D array of integers, chunk, 
 * and the pointer to a summation array, sums
 * a buffer size bufsize, and a column count, cols
 * Returns by reference the sums of each element of a given row.
 */
void sum_chunk(int *chunk, int *sums, int bufsize, int cols) {
    for (int i = 0; i < bufsize/cols; i++) {
        sums[i] = 0;
        for (int j = 0; j < cols; j++) {
            sums[i] += *(chunk + cols*i + j);
        }
    }
}
