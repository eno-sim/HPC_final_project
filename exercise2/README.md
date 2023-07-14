# Exercise 2

## What's in this directory

The current directory contains:

- `gemm.c`: a code to call the *gemm* function and to measure its performance
- `Makefile`: a Makefile to compile `gemm.c` with different libraries and precisions
- `Epyc/`: a folder containing data collected on ORFEO's EPYC nodes, with the following structure:
    - `fixed_cores/`: study of matrix size scalability. It contains two subdirectories:
    	-`close/`: contains csv files, job file and png images of the plots for each configuration using the close thread affinity policy.
    	-`spread/`: contains csv files, job file and png images of the plots for each configuration using the spread thread affinity policy.
   
    - `fixed_size/`: study of cores scalability. It contains two subdirectories:
    	-`close/`: contains csv files, job file and png images of the plots for each configuration using the close thread affinity policy.
    	-`spread/`: contains csv files, job file and png images of the plots for each configuration using the spread thread affinity policy.
    	
Same structure for Thin nodes, as follows:   
- `Thin/`: a folder containing data collected on ORFEO's THIN nodes, with the following structure:
    - `fixed_cores/`: 
    	-`close/`
    	-`spread/` 
   
    - `fixed_size/`: 
    	-`close/`
    	-`spread/`
 

The starting point for this exercise was a pre-existing, functional code named `gemm.c` (technically called `dgemm.c` but referred to as `gemm.c` in the Makefile of the original course repository - see [here][link1]). This code was designed to invoke the function using either single or double point precision from any of the three libraries, while also measuring its performance in terms of total time and *GFLOPS* (giga-floating point operations per second). The version you'll find in this directory has been slightly modified to allow the results to be written to a CSV file, with different files being used depending on the library and precision used.

We were also provided with a Makefile (see [here][link2]) designed to compile `gemm.c` using different libraries and double-point precision. We've made slight modifications to this code, enabling it to compile with single-point precision, write the results to a file, and compile the executables into the correct folder.



[link1]: https://github.com/Foundations-of-HPC/Foundations_of_HPC_2022/blob/main/Assignment/exercise2/dgemm.c
[link2]: https://github.com/Foundations-of-HPC/Foundations_of_HPC_2022/blob/main/Assignment/exercise2/Makefile

