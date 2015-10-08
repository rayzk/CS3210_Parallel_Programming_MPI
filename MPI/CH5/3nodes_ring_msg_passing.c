#include <stdio.h>
#include <string.h>
#include "mpi.h"


/* 

This program implements a logic ring blocking message communication with 3 nodes. 

Each node generate one message with it's owm ndoe information and send to the next node. 

This implementation works when system buffer is not in use.

Ring communication:
	0S1 : node0 send to node1;
	1S2 : node1 send to node2;
	2S0 : node2 send to node0;

	0R2 : node0 receive from node2;
	...
	...

Algo:
	Phase_1 : 0S1, 1R0, 2S0;
	Phase_2 : 0R2, 1S2, wait;
	Phase_3 :           2R1;

*/

void print(int rank, char msg[]);

int main (int argc, char *argv[]){

	int my_rank, num_p, tag;
	char recv_buf[50];
	char send_buf[50];
	MPI_Status status;

	MPI_Init (&argc, &argv);
	MPI_Comm_rank (MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size (MPI_COMM_WORLD, &num_p);

	/* Print the initialization of process with process_id */
	printf ("Process initiated: %d\n", my_rank);

	/* Communication ring implementation */
	if (my_rank == 0){
		print (my_rank, "starts store msg");
		strcpy (send_buf, "*** From node 0 ***");
		print (my_rank, "finishes store msg");

		print (my_rank, "starts sending");
		MPI_Send (send_buf, strlen(send_buf)+1, MPI_CHAR, 1, my_rank, MPI_COMM_WORLD);
		print (my_rank, "ends sending");

		print (my_rank, "starts receiving");
		MPI_Recv (recv_buf, 50, MPI_CHAR, 2, 2, MPI_COMM_WORLD, &status);
		print (my_rank, "ends receiving");
	}
	else if (my_rank == 1){
		print (my_rank, "starts store msg");
		strcpy (send_buf, "*** From node 1 ***");
		print (my_rank, "finishes store msg");

		print (my_rank, "starts receiving");
		MPI_Recv (recv_buf, 50, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
		print (my_rank, "ends receiving");

		print (my_rank, "starts sending");
		MPI_Send (send_buf, strlen(send_buf)+1, MPI_CHAR, 2, my_rank, MPI_COMM_WORLD);
		print (my_rank, "ends sending");
	}
	else if (my_rank == 2){
		print (my_rank, "starts store msg");
		strcpy (send_buf, "*** From node 2 ***");
		print (my_rank, "finishes store msg");

		print (my_rank, "starts sending");
		MPI_Send (send_buf, strlen(send_buf)+1, MPI_CHAR, 0, my_rank, MPI_COMM_WORLD);
		print (my_rank, "ends sending");

		print (my_rank, "starts receiving");
		MPI_Recv (recv_buf, 50, MPI_CHAR, 1, 1, MPI_COMM_WORLD, &status);
		print (my_rank, "ends receiving");		
	}

	MPI_Finalize ();
	print (my_rank, "terminated");
}

/* print function for convenient msg showing */
void print(int rank, char msg[]){
	printf ("Process %d %s ...\n", rank, msg);
}