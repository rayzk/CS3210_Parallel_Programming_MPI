/*
CS3210 - 2015/2016
Assignemnt 2 - N-body simulation - Parallel code - Parallel version
Process rank 0 is coordinator as well as a worker
A0119378U
Zhang Kai
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

#define DEBUG 0
#define debug_print(args ...) if (DEBUG) fprintf(stderr, args)

#define MAX_N 100000 	// max number of objects
#define G 6.673E-11 	// N * (m / Kg)^2
#define dt 60.0 		// sec --- duration of one timestep in seconds
#define COORDINATOR_ID 0 	// Coordinator processes id

#define INPUT_FILE "universe.in"
#define OUTPUT_FILE "universe.out"

void Work (int, int);

int main (int argc, char **argv){

	int myid;
	int i, j;
	int numWorkers; // assume number of body is multiple of number of workers

	MPI_Init (&argc, &argv);
	MPI_Comm_rank (MPI_COMM_WORLD, &myid);
	MPI_Comm_size (MPI_COMM_WORLD, &numWorkers);	// process 0 is coordinator process and also a worker process
	
	// work
	Work (myid, numWorkers);

	MPI_Finalize ();
}

void Work (int myid, int numWorkers){

	int i, j, steps;
	int next, after, before;
	int sendPos, recvPos;
	int startPos, endPos;
	int numNullBody;
	int workerid;

	int numBody, stripSize; // numBody / numWorkers
	int times; // number of time steps
	// mass, position, velocity
	double m[MAX_N], x[MAX_N], y[MAX_N], vx[MAX_N], vy[MAX_N];
	double r, rsqr, Fx, Fy, F;

	FILE* f;
	MPI_Status status;

	// read input
	f = fopen (INPUT_FILE, "rt");
	fscanf (f, "%d %d", &numBody, &times);
	if (myid == COORDINATOR_ID){
		if (numBody > MAX_N){
			printf ("Exceed Max number of objects\n");
		}
	}
	for (i = 0; i < numBody; i++) {
		fscanf(f, "%lf %lf %lf %lf %lf", x+i, y+i, vx+i, vy+i, m+i);
	}
	fclose (f);

	// check number of body
	stripSize = numBody / numWorkers;
	if ((numNullBody = (numBody % numWorkers)) != 0){
		stripSize++;
		numNullBody = numWorkers - numNullBody;
		for (i=0; i<numNullBody; i++){
			x[numBody+i] = 0.0;
			y[numBody+i] = 0.0;
			vx[numBody+i] = 0.0;
			vy[numBody+i] = 0.0;
			m[numBody+i] = 0.0;
		}
	}

	// coordinate
	if (myid == COORDINATOR_ID){
		printf ("*** Number of Body : %d\n*** Numer of Processer : %d\n*** StripSize : %d\n*** TimeSteps : %d\n", numBody, numWorkers, stripSize, times);
	}

	// do computation
	sendPos = myid * stripSize; // position in data exchange
	startPos = myid * stripSize; // position in computation 
	endPos = startPos + stripSize - 1;
	for (steps=1; steps<=times; steps++){

		// Worker do computation
		debug_print ("Worker %d : start compute %d th step\n", myid, steps);
		// for each object (except object 0 which has fixed position)
		for (i=startPos; i<=endPos; i++) {
			if (i == 0){ // except for object 0
				continue;
			}

			// get the forces exercised by the othe N-1 objects
			Fx = Fy = 0.0;
			for (j=0; j<numBody; j++) {
				if (i != j) {
					rsqr = (x[i] - x[j]) * (x[i] - x[j]) + (y[i] - y[j]) * (y[i] - y[j]);
					r = sqrt(rsqr);
					F = G * m[i] * m[j] / rsqr;
					Fx += F * (x[j] - x[i]) / r;
					Fy += F * (y[j] - y[i]) / r;
				}
			}

			// update the position using last speed
			x[i] += vx[i] * dt;
			y[i] += vy[i] * dt;

			// update the velocity using the accumulated force
			vx[i] += Fx / m[i] * dt;
			vy[i] += Fy / m[i] * dt;
		}
		debug_print ("Worker %d : finished computing %d th step\n", myid, steps);
		
		debug_print ("Worker %d : start update data at %d th step\n", myid, steps);
		// Worker exchange data in a ring mode
		for (next=1; next<numWorkers; next++){
			before = (myid - next) >= 0 ? (myid - next) : (myid - next) + numWorkers;
			after = (myid + next) % numWorkers;
			recvPos = before * stripSize;

			//debug_print ("... here ...%d\n",myid);

			// exchange x position
			MPI_Send (&x[sendPos], stripSize, MPI_DOUBLE, after, 0, MPI_COMM_WORLD);

			MPI_Recv (&x[recvPos], stripSize, MPI_DOUBLE, before, 0, MPI_COMM_WORLD, &status);

			// exchange y position
			MPI_Send (&y[sendPos], stripSize, MPI_DOUBLE, after, 1, MPI_COMM_WORLD); 

			MPI_Recv (&y[recvPos], stripSize, MPI_DOUBLE, before, 1, MPI_COMM_WORLD, &status);
	
			// sync
			MPI_Barrier (MPI_COMM_WORLD);
		}
		debug_print ("Worker %d : finish updating data at %d th step\n", myid, steps);

		// sync
		MPI_Barrier (MPI_COMM_WORLD);
	}

	// collected data
	if (myid == COORDINATOR_ID){

		debug_print ("Coordinator : start collecting data\n");
		for (workerid=1; workerid<numWorkers; workerid++){
			recvPos = workerid * stripSize;
			// receive x data
			MPI_Recv (&x[recvPos], stripSize, MPI_DOUBLE, workerid, 0, MPI_COMM_WORLD, &status);
			// receive y data
			MPI_Recv (&y[recvPos], stripSize, MPI_DOUBLE, workerid, 0, MPI_COMM_WORLD, &status);			
		}
		debug_print ("Coordinator : finished collecting data\n");

		debug_print ("Coordinator : start printing data\n");
		// write output
		f = fopen(OUTPUT_FILE, "wt");
		for (i = 0; i < numBody; i++) {
			fprintf(f, "%lE %lE\n", x[i], y[i]);
		}
		fclose(f);

	}else {
		sendPos = myid * stripSize;
		debug_print ("Processor %d : start sending data to coordinator\n", myid);
		// send x position
		MPI_Send (&x[sendPos], stripSize, MPI_DOUBLE, COORDINATOR_ID, 0, MPI_COMM_WORLD);

		// send y position
		MPI_Send (&y[sendPos], stripSize, MPI_DOUBLE, COORDINATOR_ID, 0, MPI_COMM_WORLD);
		debug_print ("Processor %d : finished sending data to coordinator\n", myid);
	}


}










