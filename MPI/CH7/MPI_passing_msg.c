/*
7.4 数据接力传送
*/

#include <stdio.h>
#include "mpi.h"

int main (int argc, char **argv){
	
	int rank, value, size;

	MPI_Status status;
	MPI_Init (&argc, &argv);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);
	MPI_Comm_size (MPI_COMM_WORLD, &size);

	do {
		if (rank == 0){
			fprintf (stderr, "\nPlease enter a new number : ");
			scanf ("%d", &value);
			fprintf(stderr, "%d 	read << (%d)\n", rank, value);
			/* pass on the message if there is more than one process */
			if (size > 1){ 
				MPI_Send (&value, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD);
				fprintf(stderr, "%d 	send (%d) >> %d\n", rank, value, rank+1);
			}
		}
		else {
			MPI_Recv (&value, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, &status);
			fprintf(stderr, "%d 	receive (%d) << %d\n", rank, value, rank-1);
			/* if this is not the last process */
			if (rank < size-1){
				MPI_Send (&value, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD);
				fprintf(stderr, "%d 	send (%d) >> %d\n", rank, value, rank+1);
			}
		}
		MPI_Barrier (MPI_COMM_WORLD);
	}while (value >= 0);

	MPI_Finalize();
}