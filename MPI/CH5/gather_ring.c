#include <stdio.h>
#include <string.h>
#include "mpi.h"

void Gather_ring (float x[], int blocksize, float y[]);

void main (){

}

void Gather_ring (float x[], int blocksize, float y[]){

	int i, p, my_rank, succ, pred;
	int send_offset, recv_offset;
	MPI_Status status;
	MPI_Request send_request, recv_request;

	MPI_Comm_size (MPI_COMM_WORLD, &p);
}