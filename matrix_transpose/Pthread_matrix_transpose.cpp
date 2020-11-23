#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <pthread.h>
#define N 8192
using namespace std;
const int numofthread = 16;
volatile double ** matrix;
double ** matrix_copy;
double ** createMatrix(){
	double **ptr = (double**)malloc(sizeof(double*) * N);
	for(int i = 0; i < N; i++){
		ptr[i] = (double*)malloc(sizeof(double) * N);
	}
	return ptr;
}
void deleteMatrix(double ** matrix){
	for(int i = 0; i < N; i++){
		free(matrix[i]);
	}
	free(matrix);
}
void randomMatrix(double ** matrix){
	srand(time(0));
	for(int i = 0; i < N; i++){
		for(int j = 0; j < N; j++){
			double fractional = (double)rand() / RAND_MAX;
			double integer = (double)rand();
			matrix[i][j] = integer + fractional;
		}
	}
}

void thread1(void *k_){
	int k = (int)k_;
	for(int i = k * N / 4; i < k * N / 4 + N / 8; i++){
		for(int j = i; j < k * N / 4 + N / 8; j++){
			double temp = matrix[i][j];
			matrix[i][j] = matrix[j][i];
			matrix[j][i] = temp;
		}
	}
	for(int i = k * N / 4 + N / 8; i < (k + 1) * N / 4; i++){
		for(int j = i; j < (k + 1) * N / 4; j++){
			double temp = matrix[i][j];
			matrix[i][j] = matrix[j][i];
			matrix[j][i] = temp;
		}
	}
}

void thread2(void *k_){
	int k = (int)k_ - 4;
	int y, x;
	for(int i = 7; i >= 1; i--){
		if(k < i){
			x = k; 
			break;
		}
		k = k - i;
		y++;
	}
	y = y * N / 8;
	x = N - N * (x + 1) / 8;
	for(int i = 0; i < N / 8; i++){
		for(int j = 0; j < N / 8; j++){
			double temp = matrix[i + x][j + y];
			matrix[i + x][j + y] = matrix[j + y][i + x];
			matrix[j + y][i + x] = temp;
		}
	}
}
double transpose(double ** matrix){
	clock_t start, finish;
	double total_time;
	pthread_t tid[16];
	start = clock();
	for(int k = 0; k < 32; k++){
		if(k < 4){
			pthread_create(&tid[k], NULL, thread1, (void*)k);
		}else{
			pthread_create(&tid[k], NULL, thread2, (void*)k);
		}
	}
	for(int k = 0; k < 32; k++){
		pthread_join(tid[k], NULL);
	}
	finish = clock();
	total_time = ((double)(finish - start)) / CLOCKS_PER_SEC;
	return total_time;
}

bool inspect(double ** matrix, double ** matrix_copy){
	for(int i = 0; i < N; i++){
		for(int j = i; j < N; j++){
			if(abs(matrix[i][j] - matrix_copy[j][i]) > 1E-6) return false;
		}
	}
	return true;
}

void memcpy_own(double ** matrix_copy, double ** matrix){
	for(int i = 0; i < N; i++){
		for(int j = 0; j < N; j++){
			matrix_copy[i][j] = matrix[i][j];
		}
	}
}

int main()
{
	matrix = createMatrix();
	matrix_copy = createMatrix();
	randomMatrix(matrix);
	transpose(matrix);
	double time_in_all = 0;
	for(int i = 0; i < 20; i++){
		randomMatrix(matrix);
		memcpy_own(matrix_copy, matrix);
		double time_current = transpose(matrix);
		if(inspect(matrix, matrix_copy)){
			printf("successfully transpose the matrix by %f seconds\n", time_current);
		}else{
			printf("fail in transpose!\n");
			return 0;
		}
		time_in_all += time_current;
	}
	printf("Average time for transpose:%f seconds\n", time_in_all / 20);
	deleteMatrix(matrix);
	deleteMatrix(matrix_copy);
	return 0;
}
