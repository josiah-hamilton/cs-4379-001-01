#include<mpi.h>
#include<stdio.h>
#include<stdlib.h> // rand(), srand()
#include<time.h>

#define RANKS 4
#define ROWS 32
#define COLS 32
#define CHUNKSIZE ROWS / RANKS

//int rank0();        // status
//int rankchild();    // status
void sum_chunk(int*,int*);

int main(int argc, char** argv) {
    srand(time(NULL)); // seed clock for rand()

    int size;    // needed for sending # of processes to MPI
    int myrank;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int rank = myrank * CHUNKSIZE;
    int chunkbufsize = sizeof(int)*COLS*CHUNKSIZE;
    int sumbufsize   = sizeof(int)*CHUNKSIZE;

    // rank 0 generates matrix data and sums first chunk
    if (myrank == 0) {

        // Allocate row pointers + rows
        int *table  = (int*) calloc(ROWS      * COLS,sizeof(int *));
        int *chunk0 = (int*) calloc(CHUNKSIZE * COLS,sizeof(int *));
        int *chunk1 = (int*) calloc(CHUNKSIZE * COLS,sizeof(int *));
        int *chunk2 = (int*) calloc(CHUNKSIZE * COLS,sizeof(int *));
        int *chunk3 = (int*) calloc(CHUNKSIZE * COLS,sizeof(int *));
        
        // random matrix data
        for (int i = 0; i < COLS; i++) {
            for (int j = 0; j < ROWS; j++) {
                *(table + COLS*i + j) = j+i; //(int)rand();
            }
        }
        
        // split table into chunks
        for (int i = 0; i < CHUNKSIZE; i++) {
            for ( int j = 0; j < COLS; j++) {
                *(chunk0 + COLS*i + j) = *(table + (COLS*i) + j);
                *(chunk1 + COLS*i + j) = *(table + (COLS*i+1*CHUNKSIZE) + j);
                *(chunk2 + COLS*i + j) = *(table + (COLS*i+2*CHUNKSIZE) + j);
                *(chunk3 + COLS*i + j) = *(table + (COLS*i+3*CHUNKSIZE) + j);
            }
        }

        MPI_Request request1,request2,request3,request4,request5,request6;
        MPI_Status  status1,status2,status3,status4,status5,status6;
        int flag1, flag2, flag3, flag4, flag5, flag6;
        
        // The Sends and receives need unique tag number
        
        MPI_Isend(chunk1,chunkbufsize,MPI_INT,1,1,MPI_COMM_WORLD,&request1);
        MPI_Isend(chunk2,chunkbufsize,MPI_INT,2,2,MPI_COMM_WORLD,&request2);
        MPI_Isend(chunk3,chunkbufsize,MPI_INT,3,3,MPI_COMM_WORLD,&request3);

        int* sums0 = (int*)calloc(CHUNKSIZE,sizeof(int));
        int* sums1 = (int*)calloc(CHUNKSIZE,sizeof(int));
        int* sums2 = (int*)calloc(CHUNKSIZE,sizeof(int));
        int* sums3 = (int*)calloc(CHUNKSIZE,sizeof(int));

        MPI_Wait(&request1,&status1);
        MPI_Wait(&request2,&status2);
        MPI_Wait(&request3,&status3);

        // rank0 pulls its weight
        sum_chunk(chunk0,sums0);
        for (int j = 0; j < CHUNKSIZE; j++) {
            printf("%d: %d\n",myrank,*(sums0 + j));
        }

        MPI_Irecv(sums1,sumbufsize,MPI_INT,1,4,MPI_COMM_WORLD,&request4);
        MPI_Irecv(sums2,sumbufsize,MPI_INT,2,5,MPI_COMM_WORLD,&request5);
        MPI_Irecv(sums3,sumbufsize,MPI_INT,3,6,MPI_COMM_WORLD,&request6);

        MPI_Wait(&request4,&status4);
        printf("done on 4\n");
        MPI_Wait(&request5,&status5);
        printf("done on 5\n");
        MPI_Wait(&request6,&status6);
        printf("done on 6\n");
        for (int j = 0; j < CHUNKSIZE; j++) {
            printf("%d: %d\n",myrank,*(sums1 + j));
        }
        for (int j = 0; j < CHUNKSIZE; j++) {
            printf("%d: %d\n",myrank,*(sums2 + j));
        }
        for (int j = 0; j < CHUNKSIZE; j++) {
            printf("%d: %d\n",myrank,*(sums3 + j));
        }
        //free(table);
        //free(chunk0);
        //free(chunk1);
        //free(chunk2);
        //free(chunk3);
        //free(sums0);
        //free(sums1);
        //free(sums2);
        //free(sums3);
        

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

        MPI_Request request,request2;
        MPI_Status status,status2;

        int *chunk = (int*) calloc(CHUNKSIZE * COLS,sizeof(int *));
        int *sums  = (int*) calloc(CHUNKSIZE,sizeof(int));

        MPI_Irecv(chunk,chunkbufsize,MPI_INT, 0, myrank, MPI_COMM_WORLD, &request);
        MPI_Wait(&request,&status);

        sum_chunk(chunk,sums);

        MPI_Isend(sums,sumbufsize,MPI_INT,0,myrank+3,MPI_COMM_WORLD,&request2);
        MPI_Wait(&request2,&status2);
        printf("%d postsend\n",myrank);

        //free(chunk);
        //free(sums);
    }

    printf("%d: finish\n",myrank);
    MPI_Finalize();
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
 * Row-wise summation of entire chunk, 
 * takes as its inputs a 2D array of integers, chunk, 
 * and the pointer to a summation array, sums
 * (for debugging also takes the rank identifier)
 * Returns by referense the sums of each element of a given row.
 */
void sum_chunk(int *chunk, int *sums) {
    sums[0]=0;
    int i,j;
    for (i = 0; i < CHUNKSIZE; i++) {
        for (j = 0; j < COLS; j++) {
            sums[i] += *(chunk + COLS*i + j);
        }
    }
}
