#include "mpi.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define DEBUG 0
#define debug_print(args ...) if (DEBUG) fprintf(stderr, args)

#define MAX_MAT_SIZE 560
#define MASTER_ID 0

int matSize;

void InitMatrix (void);
void Master (int, int);
void Worker (int, int, int);

int main (int argc, char **argv){

	srand (0);

	int myid;
	int numWorkers; 	/* assume the matrix size is multiple of numWorkers */
	int stripSize; 	/* matSize / numWorkers */

	MPI_Init (&argc, &argv);
	MPI_Comm_rank (MPI_COMM_WORLD, &myid);
	MPI_Comm_size (MPI_COMM_WORLD, &numWorkers);
	numWorkers--; 	/* one master */

	matSize = atoi (argv[1]);
	stripSize = matSize / numWorkers;
	if (matSize % numWorkers != 0){
		debug_print ("Matrix size must be a multiple of number of workers\n");
	}else if (matSize > MAX_MAT_SIZE){
		debug_print ("Matrix size must be smaller then %d\n", MAX_MAT_SIZE);
	}

	/* Master node */
	if (myid == MASTER_ID){
		debug_print ("1 Coordinator and %d workers\n", numWorkers);
		debug_print ("Matrix size : %d\nStripSize : %d\n", matSize, stripSize);
		Master (numWorkers, stripSize);
	}else { 	/* worker node */
		Worker (myid, numWorkers, stripSize);
	}
	MPI_Finalize ();
	return 0;
}

/* Master Functions */

/* initMat : initialize matrix */
void initMat (double (*mat)[matSize]){
	int i, j;
	for (i=0; i<matSize; i++)
		for (j=0; j<matSize; j++)
			*(*(mat+i)+j) = rand() %10;
}

/* Master : mpi-code for master node */
void Master (int numWorkers, int stripSize){

	int i, j, startrow, endrow;
	int workerid;
	MPI_Status status;
	FILE *results;
	double matA[matSize][matSize], matB[matSize][matSize], matC[matSize][matSize];

	initMat (matA);
	initMat (matB);
	//initMat (matC, matSize);

	// distribute matrix A
	for (workerid=1; workerid<=numWorkers; workerid++){
		startrow = (workerid - 1) * stripSize;
		endrow = startrow + stripSize - 1;
		for (i=startrow; i<=endrow; i++){
			MPI_Send (&matA[i][0], matSize, MPI_DOUBLE, workerid, 0, MPI_COMM_WORLD);
		}
		debug_print ("Send matrix A to worker %d\n", workerid);
	}

	// distribute matrix B and get result
	for (i=0; i<matSize; i++){
		MPI_Bcast (&matB[i][0], matSize, MPI_DOUBLE, MASTER_ID, MPI_COMM_WORLD);
		debug_print ("Master broadcast Matrix B to worker\n");
		// receive result
		for (workerid=1; workerid<=numWorkers; workerid++){
			MPI_Recv (&matC[i][(workerid - 1) * 2], stripSize, MPI_DOUBLE, workerid, 0, MPI_COMM_WORLD, &status);
			debug_print ("Received result from worker %d on %dth row\n", workerid, i);
		}
	}

	results = fopen ("results", "w");
	for (i=0; i<matSize; i++){
		for (j=0; j<matSize; j++){
			fprintf(results, "%f ", matC[i][j]);
		}
		fprintf(results, "\n");
	}
}

/* Worker Functions */ 

void initResult (double *result, int size){
	int i;
	for (i=0; i<size; i++)
		*(result + i) = 0.0;
}

void Worker (int myid, int numWorkers, int stripSize){

	int i, j, k;
	double result[stripSize];
	MPI_Status status;
	double matA[stripSize][matSize];
	double column[matSize];

	// receiving data of matrix A
	for (i=0; i<stripSize; i++){
		MPI_Recv (&matA[i][0], matSize, MPI_DOUBLE, MASTER_ID, 0, MPI_COMM_WORLD, &status);
	}
	debug_print ("Worker %d received matrix A from master\n", myid);

	// do computation 
	for (i=0; i<matSize; i++){
		// initialize result array
		initResult (&result[0], stripSize);
		debug_print ("... worker %d initialized result array\n", myid);

		// receive column of matrix B
		MPI_Bcast (&column[0], matSize, MPI_DOUBLE, MASTER_ID, MPI_COMM_WORLD);
		debug_print ("Worker %d receive column %d of matrix B from Master\n", myid, i);

		for (j=0; j<stripSize; j++){
			for (k=0; k<matSize; k++){
				result[j] += matA[j][k] * column[k];
			}
		}
		MPI_Send (&result[0], stripSize, MPI_DOUBLE, MASTER_ID, 0, MPI_COMM_WORLD);
		debug_print ("Worker %d send result of column %d back\n", myid, i);
	}

}

