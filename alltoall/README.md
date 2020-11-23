# task 4

implement all-to-all by MPI

I implement the alltoall function with 2^n processes. The data type is int.

Additional 2p space is required for each process, which is recorded as send_memory and recv_memory, which can pass p / 2 elements per process.

### **Specific implementation:**

-Initialization operation: assign yourself, send your own element, and the memory array named recv_memory is initialized to the transmit array;

-Set all odd bits of the recv_memory array are moved to the second half of the array (to send all odd or even bits together), and the result is assigned to send_memory；

-Main operation: the broadcast tree operation is realized by using binary system. Considering two processes that are currently exchanged for a certain binary bit of the process number, the first half or the second half of the memory array is sent according to the parity of the binary. The two memories are used to avoid assignment conflict. Each process sends P / 2 elements, accepts P / 2 elements, and retains the previous P / 2 elements. Therefore, the number of transfers per process is P / 2;

-Update recv_memory which is the value of the final result. For a total of log(P) cycles, each update 2^k elements, plus an element in initialization, a total of P elements are updated, which is what we expect;

-If the cycle does not end, go to step 2;

compile:

`mpicc my_alltoall.c -o my_alltoall`

run:

`mpirun -np #processes(2^n^) my_alltoall`


