#include "mpi.h"
#include <stdio.h>
#include <math.h>

void main (argc, argv)

int argc;
char *argv[];

{
	int myid, numprocs; // myid is the identifier of process
						// numprocs is the number of processors
	int namelen;
	char processor_name[MPI_MAX_PROCESSOR_NAME];	// MPI_MAX_PROCESSOR_NAME defines 
												//the maximum length of nodes
	// must start with MPI_Init
	MPI_Init(&argc, &argv);	
	// get the identifier of running processor copy it into myid
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);	
	// get the number of all running processor copy it into numprocs
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);	
	MPI_Get_processor_name(processor_name, &namelen);

	fprintf(stderr, "Hello World! Process %d of %d on %s\n", myid,
	numprocs, processor_name);
	
	// must end with MPI_Finalize
	MPI_Finalize();	
}

