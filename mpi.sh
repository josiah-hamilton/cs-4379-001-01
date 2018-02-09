#!/bin/sh
#$ -V
#$ -cwd
#$ -S /bin/bash
#$ -N MPI_Test_Job
#$ -o $JOB_NAME.o$JOB_ID
#$ -e $JOB_NAME.e$JOB_ID
#$ -q west
#$ -pe west 12
#$ -P hrothgar

module load gnu openmpi

mpirun --machinefile machinefile.$JOB_ID -np $NSLOTS ./a.out
echo $JOB_NAME $JOB_ID
