# Final project FHPC 2022/2023

All the material related to this project can be found at https://github.com/Foundations-of-HPC/Foundations_of_HPC_2022/tree/main/Assignment).


## What you will find in this repository

- `Report.pdf`: This document provides a comprehensive description of the steps taken to fulfill the assignment.
- `exercise1/`: This directory houses all files associated with the first exercise.
- `exercise2/`: This directory comprises all files pertinent to the second exercise.

For details on the contents of `exercise1/` and `exercise2/` directories, as well as instructions on how to replicate the results, please refer to the respective README.md files within each directory.


## Assignment Description


### Exercise 1

For this exercise, I needed to make a parallel version of Conway's Game of Life and test how well it works when I change the conditions. The assignment asked for a **hybrid MPI/OpenMP** approach, which means I used both shared and distributed memory in the same code.

What I did was, I made an MPI version of the Game of Life where the work was shared equally between the processes. Each MPI process then created multiple OpenMP threads to get its work done faster. To see how well this scaled, I compiled and ran the program on ORFEO, which is a computer cluster at Area Science Park in Trieste.

You can get more details about this task from [this document][link1] in the main course repository.


### Exercise 2

The second exercise tasked me with benchmarking the performance of three HPC mathematical libraries: **MKL**, **openBLAS**, and **BLIS**. For the latter, I had to download and compile it myself in my dedicated workspace on ORFEO.

Specifically, the task was to compare how well the gemm function from level 3 BLAS performs on matrix-matrix multiplications. The comparisons were done under different conditions: as the size of the matrix increases (while keeping the number of CPUs constant), and as the number of CPUs increases (while keeping the matrix size constant). These tests were carried out on both **EPYC** and **THIN** nodes of ORFEO. They were also done for single and double precision floating point numbers, and under different thread allocation policies (I chose to use close cores and spread cores).

More information about this exercise can be found in [this document][link2] in the main course repository.




[link1]: https://github.com/Foundations-of-HPC/Foundations_of_HPC_2022/blob/main/Assignment/exercise1/Assignment_exercise1.pdf
[link2]: https://github.com/Foundations-of-HPC/Foundations_of_HPC_2022/blob/main/Assignment/README.MD#exercise-2--comparing-mkl-openblas-and-blis-on-matrix-matrix-multiplication

