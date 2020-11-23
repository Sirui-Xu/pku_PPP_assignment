#include <stdio.h>
#include "mpi.h"
#include <stdlib.h>
#include <string.h>
int my_mpi_bcast(void* message, int count, MPI_Datatype datatype, int root, MPI_Comm comm){
	int tag = 0;
	int i;
	int my_rank;
	int p;
	MPI_Status status;
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	//printf("process %d enter into bcast\n", my_rank);
	int delta = (my_rank - root + p) % p;

	if(delta != 0){
		MPI_Recv(message, count, datatype, ((delta & (delta - 1)) + root) % p, tag, comm, &status);
		//printf("process %d recv from %d\n", my_rank, ((delta & (delta - 1)) + root) % p);
		i = (delta & (~delta + 1)) >> 1;
	}else{
		i = 1 << 30;
	}

	for(; i > 0; i >>= 1){
		if((i | delta) < p){
			MPI_Send(message, count, datatype, (root + (i | delta)) % p, tag, comm);
			//printf("process %d sent to %d\n", my_rank, (root + (i | delta)) % p);
		}
	}

	return 1;
}
int main(int argc, char** argv){
	int my_rank;
	int p;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	int N = 0;
	char message[100];
	if(my_rank == 0){
		printf("please input a sentence (less than 100 char)\n");
		gets(message);
		N = strlen(message) + 1;
	}
	my_mpi_bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

	my_mpi_bcast(message, N, MPI_CHAR, 0, MPI_COMM_WORLD);

	printf("process %d receive: %s\n", my_rank, message);

	MPI_Finalize();
	return 0;
}

