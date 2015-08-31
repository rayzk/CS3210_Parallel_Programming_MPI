#include <stdio.h>
#include <string.h>
#include "mpi.h"

/*
************************ UnFinished **********************************

This program implements 4 nodes communication.
Each node i send its node_id to the next i+1 node. After receiving msg, recv_node multiple the msg by 2 and send the result back.

MPI_Isend() and MPI_Irecv() is used.

*/

void print (int rank, char msg[]);

int main (int argc, char *argv[]){

	int my_rank, num_p, tag;
	char recv_buf[50];
	char send_buf[50];
	char send_buf_local[50];
	char recv_buf_local[50];

	MPI_Request send_request, recv_request;
	MPI_Status status;

	MPI_Init (&argc, &argv);
	MPI_Comm_rank (MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size (MPI_COMM_WORLD, &num_p);

	/* Print the initialization of process with process_id */
	printf ("Process initiated: %d\n", my_rank);

	/* Communication ring implementation */
	for (int i=0; i<num_p; i++){
		if (my_rank == i){
			print (my_rank, "starts store msg");
			strcpy (send_fuf_local, my_rank);
			print (my_rank, "finishes store msg");

			print (my_rank, "sending");
			MPI_Isend (send_buf_local, strlen(send_fuf_local)+1, MPI_CHAR, i!=(num_p-1) ? i+1 : 0, i!=(num_p-1) ? i+1 : 0, MPI_COMM_WORLD, &send_request);
			print (my_rank, "finishes sending");

			print (my_rank, "receiving msg");
			MPI_Irecv(recv_buf, strlen(recv_buf)+1, MPI_CHAR, i!=(0) ? i-1 : num_p-1: 0, i!=(0) ? i-1 : num_p-1, MPI_COMM_WORLD, &recv_request);
			print (my_rank, "finishes receiving");

			print (my_rank, "starts multipling");
			print (my_rank, "finishes multipling");

			print (my_rank, "sending back");
			print (my_rank, "finishes sending back");

			print (my_rank, "receiving old msg");
			MPI_Irecv();
			print (my_rank, "finishes receiving old msg");

			print (my_rank, "checking");
			if ()
		}else {
			continue;
		}
	}
}