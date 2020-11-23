#include <stdio.h>
#include "mpi.h"
#include <stdlib.h>
#include <string.h>

void my_alltoall(int* send_data, int* recv_data, MPI_Comm communicator)
{
	int my_rank;
	int p; //p = 2^n
	MPI_Status status;
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	int *send_memory = (int*)malloc(sizeof(int) * p);
	int *recv_memory = (int*)malloc(sizeof(int) * p);
    //initialize
	memcpy(recv_memory, send_data, sizeof(int) * p);
	recv_data[my_rank] = recv_memory[my_rank];
	
	for(int cnt = 1, i = 1; i < p; i <<= 1, cnt++){		
		/* Shift the odd number of the array to the last half */
		for(int j = 0; j < p; j++){
			if(j % 2 == 0){
				send_memory[j / 2] = recv_memory[j];
			}else{
				send_memory[(j + p) / 2] = recv_memory[j];
			}
		}
		memcpy(recv_memory, send_memory, sizeof(int) * p);
		
		/* Exchange elements in the current two processes */
		if((i & my_rank) > 0){
			MPI_Send(send_memory, p / 2, MPI_INT, i ^ my_rank, 0, communicator);
			//printf("process %d send to process %d\n", my_rank, i^my_rank);
			MPI_Recv(recv_memory, p / 2, MPI_INT, i ^ my_rank, 0, communicator, &status);
			//printf("process %d recieve from process %d\n", my_rank, i^my_rank);
			//for(int k = 0; k < p; k++) printf("%d ", recv_memory[k]);
			//printf("\n");
		}else{
			MPI_Recv(recv_memory + p / 2, p / 2, MPI_INT, i ^ my_rank, 0, communicator, &status);
			//printf("process %d recieve from process %d\n", my_rank, i^my_rank);
			MPI_Send(send_memory + p / 2, p / 2, MPI_INT, i ^ my_rank, 0, communicator);
			/*printf("process %d send to process %d\n", my_rank, i^my_rank);
			for(int k = 0; k < p; k++) printf("%d ", recv_memory[k]);
			printf("\n");*/ 
		}
		
		/* Write elements in memory into recv_data */
		for(int j = 0; j < (i<<1); j++){
			recv_data[((~((i<<1) - 1)) & my_rank) + j] = recv_memory[(p / (i<<1)) * j + (my_rank >> cnt)]; 
			//printf("process %d data[%d] = %d\n", my_rank, ((~((i<<1) - 1)) & my_rank) + j, recv_memory[(p / (i<<1)) * j + (my_rank >> cnt)]);
		}
	}
	
	free(send_memory);
	free(recv_memory);
	return;
}

int main(int argc, char** argv){
	int my_rank;
	int p;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	int *a = (int*)malloc(sizeof(int)*p);
	int *b = (int*)malloc(sizeof(int)*p);
	
	for(int i = 0; i < p; i++){
		a[i] = p * my_rank + i;
	}
	
	my_alltoall(a, b, MPI_COMM_WORLD);
	
	/* check for answers */
	int flag = 1;
	for(int i = 0; i < p; i++){
		if(b[i] != p * i + my_rank){
			flag = 0;
			break;
		}
	}
	
	if(flag == 1){
		printf("process %d recieve right data\n", my_rank);
	}else{
		printf("process %d recieve wrong data\n", my_rank);
	}
	
	free(a);
	free(b);
	MPI_Finalize();
	return 0;
}
