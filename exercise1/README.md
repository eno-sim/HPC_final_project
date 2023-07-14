# Exercise 1
The objective of this exercise is to create a parallel version of Conway's Game of Life using MPI and OpenMP. Known as a zero-player game, the evolution of the Game of Life relies solely on its initial setup. For this project, I've crafted two distinct evolution methodologies:

- Static evolution: In this technique, the system is 'frozen' at every state $s_i$, after which the new cell status $s_{i+1}$ is computed based on the system state at $s_i$.

- Ordered evolution: In this approach, evolution starts from the cell in position (0,0) and proceeds row by row.


## What's in this directory

This directory contains:

- `Makefile`: a makefile to compile all codes in `src/`

- `main.c`: contains the hybrid MPI+OpenMP code to implement the Game of Life.

- `GameofLife_serial.c`: serial version of the Game of Life, used to test the correctness of the    parallel version.


- `src/`: a folder containing Game of Life source codes:
    - `static_evolution.c`: Implements the static evolution
    - `ordered_evolution.c`: Implements the ordered evolution
    - `read_write_pgm.c`: Reads from and writes to a `.pgm` file


- `Epyc/`: a folder containing data collected on ORFEO's EPYC nodes:
    - `MPI_strong_scalability/`: contains `job.sh` file, .csv files that hold the performance results and a .png of the plot. All these files are related to MPI strong scalability study on Epyc nodes.
    - `OMP_scalability/`: contains `job.sh` file, .csv files that hold the performance results and a .png of the plot. All these files are related to OpenMP scalability study on Epyc nodes.
    - `MPI_weak_scalability`: contains `job.sh` file, .csv files that hold the performance results and a .png of the plot. All these files are related to MPI weak scalability study on Epyc nodes.
    
- `Thin/`: a folder containing data collected on ORFEO's THIN nodes
    - `MPI_strong_scalability/`:  contains `job.sh` file, .csv files that hold the performance results and a .png of the plot. All these files are related to MPI strong scalability study on Thin nodes.
    - `OMP_scalability/`: contains `job.sh` file, .csv files that hold the performance results and a .png of the plot. All these files are related to OpenMP scalability study on Thin nodes.
    - `MPI_weak_scalability`: contains `job.sh` file, .csv files that hold the performance results and a .png of the plot. All these files are related to MPI weak scalability study on Thin nodes.
    
- `include/`: a folder containing the header files:
    - `static_evolution.h`: header file for the functions defined inside `static_evolution.c`.
    - `ordered_evolution.h`: header file for the functions defined inside `ordered_evolution.c`.
    - `read_write_pgm.h`: header file for the functions defined inside `read_write_pgm.c`.



## Compilation and how to run the code

To compile and run the program, after carefully modifying the `job.sh` file according to your needs and connecting to ORFEO:

```bash
sbatch job.sh
```
In the job file, the version of MPI being loaded is `openMPI/4.1.5/gnu/12.2.1`. 
The program is executed using `mpirun`. Below are the input parameters:

- `-f`: This specifies the file name to write/read from (always required)
- `-k`: This sets the size of the world that you want to initialize 
- `-s`: Determines the number of steps after which you want to save a snapshot of the world (default value is 0, which only saves the output)
- `-i`: Initializes the playground
- `-r`: Runs the game
- `-n`: Sets the number of generations
- `-e`: Chooses the evolution policy. The options are:
    - 0 for ordered evolution
    - 1 for static evolution

  



