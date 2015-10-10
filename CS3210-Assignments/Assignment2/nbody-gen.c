/**
 * CS3210 2015/2016
 * Assignment 2 - N-body generator
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define MAX_N		10000		// max no of objects

#define MAX_T		10000000	// max time steps

#define G 		6.673E-11	// N*(m/kg)^2

#define M		1.99E30		// mass of first object

int main(int argc, char** argv) {
	int N, T, i;
	double m, r, v;

	if (argc < 3) {
		printf("Usage: %s N T\n", argv[0]);
		return 1;
	}

	N = atol(argv[1]);
	if (N > MAX_N) {
		printf("Number of objects is too big (%d)! Reducing to %d!\n", N, MAX_N);
		N = MAX_N;
	} 

	T = atol(argv[2]);
	if (T > MAX_T) {
                printf("Number of time steps is too big (%d)! Reducin to %d!\n", T, MAX_T);
                T = MAX_T;
        }

	FILE* f = fopen("universe.in", "wt");
	fprintf(f, "%d %d\n", N, T);
	fprintf(f, "%lE %lE %lE %lE %lE\n", 0.0, 0.0, 0.0, 0.0, M);

	srand((unsigned int)time(NULL));
	for (i = 1; i < N; i++) {
		// generate mass
		m = (10.0 + rand()/(float)(RAND_MAX) * 1000.0) * 1.0E24;
		// generate distance
		r = (10.0 + rand()/(float)(RAND_MAX) * 100000.0) * 1.0E9;
		v = -1.0 * sqrt(2 * G * M / r);
		fprintf(f, "%lE %lE %lE %lE %lE\n", r, 0.0, 0.0, v, m);
	}

	fclose(f);

	return 0;
}
