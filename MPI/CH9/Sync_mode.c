#include <stdio.h>
#include "mpi.h"

#define SIZE 10 	/* buffer size */

static int src = 0;
static int dest = 1;

int main (int argc, char **argv){

	int rank;
	int act_size = 0;
	int flag, np, rval, i;
	int buffer[SIZE];
	MPI_Status status, status1, status2;
	int count1, count2;

	MPI_Init (&argc, &argv);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);
	MPI_Comm_size (MPI_COMM_WORLD, &np);

	if (np != 2){
		fprintf (stderr, "*** This program use exactly 2 processes\n");
		MPI_Abort (MPI_COMM_WORLD, 1);
	}

	act_size = 5; 	/* longest msg length */
	if (rank == src){
		act_size = 1;
		MPI_Ssend (buffer, act_size, MPI_INT, dest, 1, MPI_COMM_WORLD);
		fprintf (stderr, "MPI_Ssend %d data, tag = 1\n", act_size);
		act_size = 4;
		MPI_Ssend (buffer, act_size, MPI_INT, dest, 2, MPI_COMM_WORLD);
		fprintf (stderr, "MPI_Ssend %d data, tag = 2\n", act_size);
	}else if (rank == dest){
		MPI_Recv (buffer, act_size, MPI_INT, src, 1, MPI_COMM_WORLD, &status1);
		MPI_Recv (buffer, act_size, MPI_INT, src, 2, MPI_COMM_WORLD, &status2);
		MPI_Get_count (&status1, MPI_INT, &count1);
		fprintf(stderr, "receive %d data, tag = %d\n", count1, status1.MPI_TAG);
		MPI_Get_count (&status2, MPI_INT, &count2);
		fprintf(stderr, "receive %d data, tag = %d\n", count2, status2.MPI_TAG);
	}
	MPI_Finalize ();
}