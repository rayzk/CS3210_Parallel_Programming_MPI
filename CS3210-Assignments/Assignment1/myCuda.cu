#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>

#define BLOCK_SIZE 32

typedef struct {
	int width;
	int height;
	float* elements;
	int stride;
}Matrix;


// ============================= Cuda device functions =====================================//


/* allocateMatInDeviceMemory : allocate memory space in device memory with the same size as given matrix */
Matrix allocateMatInDeviceMemory (Matrix A){
	Matrix A_alloc;
	A_alloc.width = A_alloc.stride = A.width;
	A_alloc.height = A.height;
	size_t size = A.width * A.height * sizeof(float);
	cudaMalloc (&A_alloc.elements, size);

	return A_alloc;
}


/* cpyFromHostToDeviceMemory : copy data from CPU memory to GPU device memory */
void cpyMatFromHostToDevice (Matrix A_host, Matrix A_device){
	size_t size = A_host.width * A_host.height * sizeof(float);
	cudaMemcpy (A_device.elements, A_host.elements, size, cudaMemcpyHostToDevice);
}

/* cpyMatFromDeviceToHost : copy matrix from device memory to CPU memory */
void cpyMatFromDeviceToHost (Matrix A_device, Matrix A_host){
	size_t size = A_device.width * A_device.height * sizeof(float);
	cudaMemcpy (A_host.elements, A_device.elements, size, cudaMemcpyDeviceToHost);
}

/* GetElement : get a matrix element */
__device__ float GetElement (const Matrix A, int row, int col){
	return A.elements[row * A.stride + col];
}


/* SetElement : set a matrix element */
__device__ void SetElement (Matrix A, int row, int col, float value){
	A.elements[row * A.stride + col] = value;
}


/* GetSubMatrix : get BLOCK_SIZE by BLOCK_SIZE submatrix of A such that the upper-left corner of A is in row and col of original matrix */
__device__ Matrix GetSubMatrix (Matrix A, int row, int col) {
	
	Matrix A_sub;
	A_sub.width = BLOCK_SIZE;
	A_sub.height = BLOCK_SIZE;
	A_sub.stride = A.stride;
	A_sub.elements = &A.elements[A.stride * BLOCK_SIZE * row + BLOCK_SIZE * col];

	return A_sub;
}


/* mmKernel : multiplication running on GPU core */
__global__ void mmKernel (Matrix A, Matrix B, Matrix C){

	// block row and col
	int blockRow = blockIdx.y;
	int blockCol = blockIdx.x;

	// each thread block computes one sub-matrix of C
	Matrix C_sub = GetSubMatrix (C, blockRow, blockCol);

	// accumulating result 
	float Cvalue = 0.0;

	// thread row and col wihtin C_sub
	int row = threadIdx.y;
	int col = threadIdx.x;

	for (int m=0; m<(A.width / BLOCK_SIZE); ++m){
 		// get sub-matrix of A, B from device memory
 		Matrix A_sub = GetSubMatrix (A, blockRow, m);
 		Matrix B_sub = GetSubMatrix (B, m, blockCol);

 		// store sub-matrix of A, B into shared memory
 		__shared__ float As[BLOCK_SIZE][BLOCK_SIZE];
 		__shared__ float Bs[BLOCK_SIZE][BLOCK_SIZE];
 		As[row][col] = GetElement (A_sub, row, col);
 		Bs[row][col] = GetElement (B_sub, row, col);
 		__syncthreads();

 		// compute multiplication
 		for (int e=0; e<BLOCK_SIZE; ++e){
 			Cvalue += As[row][e] * Bs[e][col];
 			__syncthreads();
 		}

 		// write result back to device memory
 		SetElement (C_sub, row, col, Cvalue);
	}
}


/* mm : Matrix Multiplication running on device */
void mmCuda (const Matrix A, const Matrix B, Matrix C){

	// allocate and copy matrix from CPU memory to device memory
	Matrix A_alloc = allocateMatInDeviceMemory (A);
	Matrix B_alloc = allocateMatInDeviceMemory (B);
	Matrix C_alloc = allocateMatInDeviceMemory (C);
	cpyMatFromHostToDevice (A, A_alloc);
	cpyMatFromHostToDevice (B, B_alloc);

	
	dim3 dimBlock (BLOCK_SIZE, BLOCK_SIZE);
	dim3 dimGrid (B.width / dimBlock.x, A.height / dimBlock.y);
	mmKernel<<<dimGrid, dimBlock>>> (A_alloc, B_alloc, C_alloc);
	cudaThreadSynchronize();
	cpyMatFromDeviceToHost(C_alloc, C);

	// free device memory
	cudaFree (A_alloc.elements);
	cudaFree (B_alloc.elements);
	cudaFree (C_alloc.elements);
}

// ============================= CPU functions =====================================//

float HostGetElement (const Matrix A, int row, int col){
	return A.elements[row * A.stride + col];
}

void HostSetElement (Matrix A, int row, int col, float value){
	A.elements[row * A.stride + col] = value;
}

void mm(Matrix A, Matrix B, Matrix C)
{
	int i, j, k;
	int size = A.width;
	float result;

	// Do the multiplication
	for (i = 0; i < size; i++){
		for (j = 0; j < size; j++){
			result = 0.0;
			for(k = 0; k < size; k++){
				result += HostGetElement(A, i, k) * HostGetElement(B, k, j);
			}
			HostSetElement (C, i, j, result);
		}
	}
}

// ============================= Helper Functions =====================================//


/* GetTime : get the clock time */
long long GetTime(){
#ifdef __linux__
	struct timespec tp;
	clock_gettime(CLOCK_REALTIME, &tp);
	return (long long)(tp.tv_nsec + (long long)tp.tv_sec * 1000000000ll);
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (long long)(tv.tv_usec * 1000 + (long long)tv.tv_sec * 1000000000ll);
#endif
}

/* CheckResult : compare two Matrix */
void CheckResult (Matrix A, Matrix B){
	// Compare the results
	int i, j;
	int correct = 1;
	int size = A.width;

	for (i = 0; correct && i < size; i++)
		for (j = 0; j < size; j++)
			if (HostGetElement(A, i, j) != HostGetElement(B, i, j)) {
				correct = 0;
				break;
			}

	if (correct)
		printf("The result matrices are identical!\n");
	else
		printf("Difference in result matrices at element (%d, %d)!\n", i, j);
}


void work (int size){

	Matrix A, B, C, D;
	long long before, after;

	// Initialize matrix elements
	int i, j;

	A.width = size;
	A.stride = A.height = size;
	A.elements = (float*)malloc(A.width * A.height * sizeof(float));
	B.width = size;
	B.stride = B.height = size;
	B.elements = (float*)malloc(B.width * B.height * sizeof(float));
	C.width = size;
	C.stride = C.height = size;
	C.elements = (float*)malloc(C.width * C.height * sizeof(float));
	D.width = size;
	D.stride = D.height = size;
	D.elements = (float*)malloc(D.width * D.height * sizeof(float));

	for (i = 0; i < size; i++)
		for (j = 0; j < size; j++){
			A.elements[i*A.width + j] = rand() % 10;
			B.elements[i*A.width + j] = rand() % 10;
		}
			

	/*// Perform mm on GPU
	before = GetTime ();
	mm(A, B, D);
	after = GetTime ();
	fprintf(stderr, "Matrix multiplication on CPU took %1.2f seconds\n", ((float)(after - before))/1000000000);
*/
    // Perform mm on GPU
	before = GetTime ();
	mmCuda(A, B, C);
	after = GetTime ();
	//fprintf(stderr, "Matrix multiplication on GPU took %1.2f seconds\n", ((float)(after - before))/1000000000);

	//CheckResult (C, D);
	//print_matrix(C);

}


int main(int argc, char ** argv)
{
	int size;
	srand(0); 
	//printf("Usage: %s <size>\n", argv[0]);
    
	if (argc >= 2)
		size = atoi(argv[1]);
	else
		size = 1024;
		
	//fprintf(stderr,"Sequential matrix multiplication of size %d\n", size);
    
	// Multiply the matrices
	work(size);

	return 0;
}



