# task2
Transpose large matrix in place. 

In each block, the use of share memory makes each access condensed. (cuda)

compile:

`nvcc -ccbin /usr/bin -arch=sm_35 -o matrix_transpose_cuda matrix_transpose_cuda.cu`

run:

`./matrix_transpose_cuda`
