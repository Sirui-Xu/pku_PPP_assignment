#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <cstring>
#define N 8192
using namespace std;
double ** matrix;
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
void divide_and_conquer(double ** matrix, int n, int x, int y){
	if(n == 1) return;
	for(int i = 0; i < n / 2; i++){
		for(int j = 0; j < n / 2; j++){
			double c;
			c = matrix[x + i + n / 2][y + j];
			matrix[x + i + n / 2][y + j] = matrix[x + i][y + j + n / 2];
			matrix[x + i][y + j + n / 2] = c;
		}
	}
	divide_and_conquer(matrix, n / 2, x, y);
	divide_and_conquer(matrix, n / 2, x + n / 2, y);
	divide_and_conquer(matrix, n / 2, x, y + n / 2);
	divide_and_conquer(matrix, n / 2, x + n / 2, y + n / 2);
}

double transpose(double ** matrix){
	clock_t start, finish;
	double total_time;
	start = clock();
	divide_and_conquer(matrix, N, 0, 0); 
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
