#include <stdio.h>
#include <string.h>
#include "mpi.h"

void print(int rank, char msg[]);

int main(int argc, char *argv[]){

	int my_rank, p, tag=0;
	char msg[20];
	MPI_Status status;

	/* Init : must be called before any other MPI functions */
	MPI_Init (&argc, &argv);

	/* rank of calling process */
	MPI_Comm_rank (MPI_COMM_WORLD, &my_rank);

	/* total number of processes in communicator p */
	MPI_Comm_size (MPI_COMM_WORLD, &p);
	printf ("%d processes initiated\n", p);

	if (my_rank == 0){
		strcpy (msg, "Hello");
		print (my_rank, "copying msg to system buffer");

		print (my_rank, "sending message");
		MPI_Send (msg, strlen(msg)+1, MPI_CHAR, 1, tag, MPI_COMM_WORLD);
		print (my_rank, "finished sending msg");
	}
	if (my_rank == 1){
		MPI_Recv (msg, 20, MPI_CHAR, 0, tag, MPI_COMM_WORLD, &status);
		print (my_rank, "finished receiving msg");
	}

	MPI_Finalize ();	
	print (my_rank, "terminated");
}

void print(int rank, char msg[]){
	printf ("Process %d %s ...\n", rank, msg);
}