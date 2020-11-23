/*
**
** 1700012803
** xusirui
**
*/
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#define N 8192
#define TILE_DIM 16
const int nbytes = N * N * sizeof(double);
double * matrix;
double * matrix_copy;
__global__ void Kernel_func(double * matrix_d, int n)
{
    int idx = blockDim.x * blockIdx.x + threadIdx.x;
    int idy = blockDim.y * blockIdx.y + threadIdx.y;
    int _idy = blockDim.x * blockIdx.x + threadIdx.y;
    int _idx = blockDim.y * blockIdx.y + threadIdx.x;
    if(blockIdx.x >= blockIdx.y){
    	__shared__ double tile1[TILE_DIM][TILE_DIM+1];
    	__shared__ double tile2[TILE_DIM][TILE_DIM+1];
    	tile1[threadIdx.x][threadIdx.y] = matrix_d[idy * n + idx];
    	tile2[threadIdx.x][threadIdx.y] = matrix_d[_idy * n + _idx];
    	__syncthreads();
    	matrix_d[idy * n + idx] = tile2[threadIdx.y][threadIdx.x];
    	matrix_d[_idy * n + _idx] = tile1[threadIdx.y][threadIdx.x];
    }
}
void createMatrix(){
	matrix = (double*)malloc(nbytes);
	matrix_copy = (double*)malloc(nbytes);
}
void deleteMatrix(){
	free(matrix);
	free(matrix_copy);
}
void randomMatrix(){/* randomize the matrix for each loop*/
	srand(time(0));
	for(int i = 0; i < N*N; i++){
			double fractional = (double)rand() / RAND_MAX;
			double integer = (double)rand();
			matrix[i] = integer + fractional;
			matrix_copy[i] = matrix[i];
	}
}
int inspect(){/* inspect the transpose function just for the first time */
	for(int i = 0; i < N; i++){
		for(int j = 0; j < N; j++){
			if(abs(matrix[i * N + j] - matrix_copy[j * N + i]) > 1E-3){
				printf("wrong answer at point (%d, %d)\n", i, j);
				return 0;
			}
		} 
	}
	return 1;
}
float transpose(){/* do the transpose */
	float total_time;
	cudaEvent_t start, stop;
	double *matrix_d;
	cudaMalloc((void**)&matrix_d, nbytes);
	cudaMemcpy(matrix_d, matrix, nbytes, cudaMemcpyHostToDevice);
	dim3 threadPerBlock(TILE_DIM, TILE_DIM);
	dim3 blockNum(N / TILE_DIM, N / TILE_DIM);
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	cudaEventRecord(start, 0);
	Kernel_func<<<blockNum, threadPerBlock>>>(matrix_d, N);
	cudaEventRecord(stop, 0);
	cudaEventSynchronize(stop);
	cudaEventElapsedTime(&total_time, start, stop);
	cudaMemcpy(matrix, matrix_d, nbytes, cudaMemcpyDeviceToHost);
	cudaFree(matrix_d);
	cudaEventDestroy(start);
	cudaEventDestroy(stop);
	return total_time;
}
int main()
{
	createMatrix();
	randomMatrix();
	transpose();
	if(inspect()){
		printf("successfully transpose the matrix!\n");
	}
	float time_in_all = 0;
	randomMatrix();
	transpose();
	for(int i = 0; i < 20; i++){
		float time_current = transpose();
		printf("transpose the matrix by %f ms\n", time_current);
		time_in_all += time_current;
	}
	printf("Average time for transpose:%f ms\n", time_in_all / 20);
	deleteMatrix();
	return 0;
}
