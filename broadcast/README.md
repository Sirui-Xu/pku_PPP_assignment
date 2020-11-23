# task 3

Design MPI program to "broadcast", requiring the basic "send" and "recv" model.

The broadcast tree is implemented by binary system, which improves the efficiency and reduces the amount of code. Ask to input a sentence, this sentence will be broadcast to all processes.

compile:

`mpicc my_mpi_bcast.c -o my_mpi_bcast`

run:

`mpirun -np #processes my_mpi_bcast`
