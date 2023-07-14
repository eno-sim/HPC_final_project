# Exercise 2

## Directory Contents

This directory contains the following:

- `gemm.c`: A program that calls the *gemm* function and measures its performance.
- `Makefile`: Used to compile `gemm.c` with different libraries and precisions.

The data collected on ORFEO's EPYC and THIN nodes are organized in the respective folders:

- `Epyc/` and `Thin/`: Each containing:
    - `fixed_cores/`: Contains data related to scalability with respect to matrix size. Inside it, there are two subdirectories:
    	- `close/`: Contains csv files, job files and png images for each configuration using the 'close' thread affinity policy.
    	- `spread/`: Contains csv files, job files and png images for each configuration using the 'spread' thread affinity policy.
    - `fixed_size/`: Contains data related to scalability with respect to the number of cores. Inside it, there are two subdirectories:
    	- `close/`: Contains csv files, job files and png images for each configuration using the 'close' thread affinity policy.
    	- `spread/`: Contains csv files, job files and png images for each configuration using the 'spread' thread affinity policy.

This exercise started with a functional `gemm.c` code (called `dgemm.c` but referred to as `gemm.c` in the original course repository Makefile - see [here][link1]). This code is designed to call the *gemm* function using single or double point precision from any of the three libraries and measure its performance. I modified it slightly to enable the output of results to a CSV file, differentiated by the library and precision used.

The provided Makefile (see [here][link2]) compiles `gemm.c` using different libraries and double-point precision. I made slight modifications to it, allowing for compilation with single-point precision, output of results to a file, and correct directory compilation of the executables.

[link1]: https://github.com/Foundations-of-HPC/Foundations_of_HPC_2022/blob/main/Assignment/exercise2/dgemm.c
[link2]: https://github.com/Foundations-of-HPC/Foundations_of_HPC_2022/blob/main/Assignment/exercise2/Makefile


