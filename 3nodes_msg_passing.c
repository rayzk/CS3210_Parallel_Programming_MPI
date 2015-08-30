#include <stdio.h>
#include <string.h>
#include "mpi.h"

void print(int rank, char msg[]);

int main(int argc, char *argv[]){

	int my_rank, p, tag=0;
	char msg_1[20];
	char msg_2[20];
	MPI_Status status;

	/* Init : must be called before any other MPI functions */
	MPI_Init (&argc, &argv);

	/* rank of calling process */
	MPI_Comm_rank (MPI_COMM_WORLD, &my_rank);

	/* total number of processes in communicator p */
	MPI_Comm_size (MPI_COMM_WORLD, &p);
	printf ("Processes initiated : %d\n", my_rank);

	if (my_rank == 0){
		strcpy (msg_1, "Hello");
		print (my_rank, "copying message to system buffer");

		strcpy (msg_2, "World!");
		print (my_rank, "copying message to system buffer");

		print (my_rank, "sending first message");
		MPI_Send (msg_1, strlen(msg_1)+1, MPI_CHAR, 1, my_rank, MPI_COMM_WORLD);
		print (my_rank, "finished sending first msg");

		print (my_rank, "sending second message");
		MPI_Send (msg_2, strlen(msg_2)+1, MPI_CHAR, 2, my_rank, MPI_COMM_WORLD);
		print (my_rank, "finished sending second msg");
	}
	else if (my_rank == 1){
		MPI_Recv (msg_1, 20, MPI_CHAR, 0, tag, MPI_COMM_WORLD, &status);
		print (my_rank, "finished receiving msg");

		MPI_Send (msg_1, strlen(msg_1)+1, MPI_CHAR, 3, my_rank, MPI_COMM_WORLD);
		print (my_rank, "finished sending msg");
	}
	else if (my_rank == 2){
		MPI_Recv (msg_1, 20, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		if (msg_1 == "Hello")
			print (my_rank, "finished receiving the first message");
		else 
			print (my_rank, "finished receiving the second message");

		MPI_Recv (msg_2, 20, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		if (msg_2 == "Hello")
			print (my_rank, "finished receiving the first message");
		else 
			print (my_rank, "finished receiving the second message");
	}

	MPI_Finalize ();	
	print (my_rank, "terminated");
}

void print(int rank, char msg[]){
	printf ("Process %d %s ...\n", rank, msg);
}