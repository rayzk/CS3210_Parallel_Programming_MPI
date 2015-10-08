/*
Similar as Jacobi_iteration.c
Use a logical column between worker 1 and worker n, so that each worker can use the MPI_Sendrecv() function.
*/


#include "mpi.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define MAXGRID 258 	/* max grid size */
#define CORRDINATOR 0 	/* process number of the Corrdinator */

static void Coordinator (int, int, int);
static void Worker (int, int, int, int, int);

/* main : initialize MPI, become one of the process */

int main (int argc, char **argv){

	int myid;
	int numWorkers, gridSize; 	/* assume aridSize is multiple of number of workders */
	int stripSize; 	/* gridSize / numWorkers */
	int numIters; 	/* number of iterations to execute */

	MPI_Init (&argc, &argv);
	MPI_Comm_rank (MPI_COMM_WORLD, &myid);
	MPI_Comm_size (MPI_COMM_WORLD, &numWorkers);
	numWorkers--; 	/* one coordinator, the other are workers */

	/* get command-line arguments and do a simple error check */
	gridSize = atoi (argv[1]);
	numIters = atoi (argv[2]);
	stripSize = gridSize / numWorkers;
	if (gridSize % numWorkers != 0){
		printf ("gridSize must be a multiple of number of workers\n");
		exit(1);
	}

	/* become one of the actual processes, depending on myid */
	if (myid == 0){
		printf ("1 Coordinator and %d Workers\n", numWorkers);
		printf ("gridSize : %d\n stripSize : %d\n numIters : %d\n", gridSize, stripSize, numIters);
		Coordinator (numWorkers, stripSize, gridSize);
	}else {
		Worker (myid, numWorkers, stripSize, gridSize, numIters);
	}

	MPI_Finalize (); 	/* clean up MPI */
}

/* Coordinator : gather and print results from Workers */
static void Coordinator (int numWorkers, int stripSize, int gridSize){

	double grid[MAXGRID][MAXGRID];
	int i, j, startrow, endrow;
	int workerid;
	MPI_Status status;
	FILE *results;
	double mydiff = 0.0, maxdiff = 0.0;

	for (workerid=1; workerid<=numWorkers; workerid++){
		startrow = (workerid-1) * stripSize + 1;
		endrow = startrow + stripSize - 1;
		for (i=startrow; i<=endrow; i++){
			MPI_Recv (&grid[i][1], gridSize, MPI_DOUBLE, workerid, 0, MPI_COMM_WORLD, &status);
		}
		printf ("got results from worker %d\n", workerid);
	}

	MPI_Reduce (&mydiff, &maxdiff, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	printf ("global maxdiff is %f\n", maxdiff);

	/* output the result to file "results" */
	results = fopen ("results", "w");
	for (i=1; i<=gridSize; i++){
		for (j=1; j<=gridSize; j++){
			fprintf (results, "%f ", grid[i][j]);
		}
		fprintf (results, "\n");
	}
}

/* Worker : each worker computes values in one strip, communicating with neighboring workers */
static void Worker (int myid, int numWorkers, int stripSize, int gridSize, int numIters){

	double grid[2][MAXGRID][MAXGRID];
	int i, j, iters;
	int current = 0, next = 1; 	/* current and next iteration indices */
	int left = 0, right = 0; 	/* neighboring strips above and below */
	MPI_Status status;
	double mydiff = 0.0, maxdiff, temp;

	/* initialize grid */
	/* all workers set the grid to be 0.0; all workers set the rowstart point and rowend point to be 1.0; first worker sets the first row to be 1.0; last worker sets the last row to be 1.0 */
	for (i=0; i<stripSize+1; i++){
		for (j=0; j<gridSize+1; j++){
			grid[current][i][j] = 0.0;
			grid[next][i][j] = 0.0;
		}
	}
	for (i=0; i<stripSize+1; i++){
		grid[current][i][0] = 1.0;
	    grid[current][i][gridSize+1] = 1.0;
	    grid[next][i][0] = 1.0;
	    grid[next][i][gridSize+1] = 1.0;
	}
	if (myid == 1){
		for (j=0; j<gridSize+1; j++){
			grid[current][0][j] = 1.0;
			grid[next][0][j] = 1.0;
		}
	}
	if (myid == numWorkers){
		for (j=0; j<gridSize+1; j++){
			grid[current][stripSize+1][j] = 1.0;
      		grid[next][stripSize+1][j] = 1.0;
		}
	}

	/* determine neighbors */
	if (myid > 1){
		left = (myid-2) % numWorkers + 1;
	}else {
		left = MPI_PROC_NULL;
	}
	if (myid < numWorkers){
		right = myid % numWorkers + 1;
	}else {
		right = MPI_PROC_NULL;
	}

	printf ("Worker %d initialized; left is worker %d and right is worker %d\n", myid, left, right);

	/* do the actual computation */
	for (iters = 1; iters <= numIters; iters++){
		/* exchange my boundaries with my neighbors, in a ring */
		/* send/receive from/to right in a ring */
		MPI_Sendrecv (&grid[next][stripSize][1], gridSize, MPI_DOUBLE, right, 0, &grid[next][stripSize+1][1], gridSize, MPI_DOUBLE, right, 0, MPI_COMM_WORLD, &status);
		/*send/receive from/to left in a ring */
		MPI_Sendrecv (&grid[next][1][1], gridSize, MPI_DOUBLE, left, 0, &grid[next][0][1], gridSize, MPI_DOUBLE, left, 0, MPI_COMM_WORLD, &status);

	    /* update my point */
	    for (i=1; i<=stripSize; i++){
	    	for (j=1; j<=gridSize; j++){
	    		grid[next][i][j] = (grid[current][i-1][j] + grid[current][i+1][j] + grid[current][i][j-1] + grid[current][i][j+1]) / 4;
	    	}
	    }

	    /* swap roles of grids */
	    current = next; 
	    next = 1-next;
	}

	/* send result from my current strip to the coordinator */
	for (i=1; i<=stripSize; i++){
		MPI_Send (&grid[current][i][1], gridSize, MPI_DOUBLE, CORRDINATOR, 0, MPI_COMM_WORLD);
	}

	/* compute maximum difference and reduce it with Coordinator */
	for (i=1; i<=stripSize; i++){
		for (j=1; j<=stripSize; j++){
			temp = fabs(grid[next][i][j] - grid[current][i][j]);
			if (temp > mydiff)
				mydiff = temp;
		}
	}
	MPI_Reduce (&mydiff, &maxdiff, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	printf ("maxdiff of worker %d is %f\n", myid, mydiff);
}











