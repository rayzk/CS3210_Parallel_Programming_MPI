#include "mpi.h"
#include <stdio.h>

int main (int argc, char **argv){

	int rank, size, node, buf[1];
	MPI_Status status;
	MPI_Init (&argc, &argv);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);
	MPI_Comm_size (MPI_COMM_WORLD, &size);

	if (rank == 0){
		for (node=0; node<size-1; node++){
			MPI_Recv (buf, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			printf ("msg = %d from %d with tag %d\n", buf[0], status.MPI_SOURCE, status.MPI_TAG);
		}
	}else {
		buf[0] = rank;
		MPI_Send (buf, 1, MPI_INT, 0, rank, MPI_COMM_WORLD);
	}

	MPI_Finalize();
}