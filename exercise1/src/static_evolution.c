#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include "mpi.h"
#include <omp.h>
#include "read_write_pgm.h"


#define MAXVAL 255






void update_cell_static(unsigned char *top_row, unsigned char *bottom_row,
 unsigned char *local_playground, unsigned char *updated_playground, int xsize, int ysize, int x, int y)
{
    int alive_neighbors = 0;
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            if (i == 0 && j == 0)
                continue;

            int nx = (x + i + xsize) % xsize;
            int ny = (y + j + ysize) % ysize;

            unsigned char cell_value;

            if (ny == ysize-1 && y==0)
            {
                cell_value = top_row[nx];
            }
            else if (ny == 0 && y==ysize-1)
            {
                cell_value = bottom_row[nx];
            }
            else
            {
                cell_value = local_playground[ny * xsize + nx];
            }

            alive_neighbors += (cell_value == MAXVAL);
        }
    }

    int cell_index = y * xsize + x;
    updated_playground[cell_index] = (((local_playground[cell_index]==MAXVAL) &&
     (alive_neighbors == 2 || alive_neighbors == 3)) || 
     ((local_playground[cell_index]==0) && alive_neighbors == 3)) ? MAXVAL : 0;
}



/*
STATIC EVOLUTION ALGORITHM:
  1) Initialize MPI and get the rank (process ID)
   and the number of processes.
  2) Determine the chunk size for each process by 
  dividing the y dimension of the playground 
  by the number of processes. 
  Also, determine if there is any remainder.
  3) Calculate the starting y position for each 
  process based on its rank, chunk size, 
  and the remainder from step 2.
  4) For each iteration from 0 to n:
    a. Create a local copy of the playground 
    for each process.
    b. Use OpenMP parallelization 
    within each process to update the
     cells in the local playground.
    c. Use MPI communication to 
    exchange the necessary border rows 
    between neighboring processes to ensure 
    that each process has the correct information 
    about its neighbors for the next iteration.
    d. Update the main playground with the 
    local playgrounds from each process.
    Finalize MPI.

    The root process (usually the one with rank 0)
     will have the whole playground initially, 
     but it will distribute the data among other processes 
     during the MPI_Scatterv operation. After this step, 
     each process will only hold its portion of the playground. 
 int rank, size;
    MPI_Init(NULL, NULL);
   
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); //get the rank of the current process
    MPI_Comm_size(MPI_COMM_WORLD, &size); //get the total number of processes
    printf("MPI initialized, I am process %d", rank);
      int rank, size;
    MPI_Init(NULL, NULL);
   
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); //get the rank of the current process
    MPI_Comm_size(MPI_COMM_WORLD, &size); //get the total number of processes
    printf("MPI initialized, I am process %d", rank);
        At the end of the computation, the root process will gather 
    the results from all other processes using MPI_Gatherv, 
    reconstructing the whole playground with the updated cell states.

*/


void static_evolution(unsigned char *playground, int xsize, int ysize, int n, int s)
{
    int rank, size;
   
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); //get the rank of the current process
    MPI_Comm_size(MPI_COMM_WORLD, &size); //get the total number of processes
    printf("MPI initialized, I am process %d", rank);
    int chunk = ysize / size;
    int mod = ysize % size; // extra rows to evenly distribute to the first mod processes
    int my_chunk = chunk + (rank < mod); // the first processes will get an extra row each
    int my_first = rank * chunk + (rank < mod ? rank : mod); //calculate the starting row index for the current process
    int my_last = my_first + my_chunk; //Calculate the ending row index for the current process (DELETABLE)
    int local_size = my_chunk * xsize; 
    
    unsigned char *local_playground = (unsigned char *)malloc(local_size * sizeof(unsigned char));
    
    int *sendcounts = NULL; //array of integers containing how many bytes are to be sent to each process,
    //the index of the array identifies the rank of the process
    
    int *displs = NULL; //it will indicate the starting position on the playground of the chunk assigned
    //to the process

    if (rank == 0)
    {
        sendcounts = (int *)malloc(size * sizeof(int));
        displs = (int *)malloc(size * sizeof(int));
        for (int i = 0; i < size; i++)
        {
            sendcounts[i] = (chunk + (i < mod)) * xsize;
            displs[i] = i * chunk * xsize + (i < mod ? i : mod) * xsize;
        }
    }
    // integer array (of length group size).
    // Entry i specifies the displacement 
    // (relative to sendbuf from which to take the outgoing data to process i)

    MPI_Scatterv(playground, sendcounts, displs, MPI_UNSIGNED_CHAR, local_playground, 
    local_size, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

    for (int step = 1; step <= n; step++)
    {
        unsigned char *top_ghost_row = (unsigned char *)malloc(xsize * sizeof(unsigned char));
        unsigned char *bottom_ghost_row = (unsigned char *)malloc(xsize * sizeof(unsigned char));


       unsigned char *updated_playground = (unsigned char *)malloc(local_size * sizeof(unsigned char));


        int top_neighbor = (rank - 1 + size) % size; // always rank-1 except for when rank==0, where
        // the result of the modulus is size-1 (the last process in rank order is the top neighbor of process 0)
        
        
        int bottom_neighbor = (rank + 1) % size; // (rank + 1) / size is always 0, so the modulus will always
        //be rank+1, except for when rank == size-1 (maximum) where the modulus is 0 (process 0 is the bottom 
        //neighbor of the last process)
        
        
        
       // Each process sends its top row to its top neighbor and receives its bottom ghost row
       // from its bottom neighbor
        MPI_Sendrecv(&local_playground[0], xsize, MPI_UNSIGNED_CHAR, top_neighbor, 0,
                     bottom_ghost_row, xsize, MPI_UNSIGNED_CHAR, bottom_neighbor, 0, 
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);



        // Each process sends its bottom row to its bottom neighbor and receives its top ghost row
        // from its top neighbor. 
        MPI_Sendrecv(&local_playground[(my_chunk - 1) * xsize], xsize, MPI_UNSIGNED_CHAR, bottom_neighbor, 1,
                     top_ghost_row, xsize, MPI_UNSIGNED_CHAR, top_neighbor, 
                     1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

       
       
       #pragma omp parallel for collapse(2) 
       for (int y = 0; y < my_chunk; y++)
         {
            for (int x = 0; x < xsize; x++)
            {   
                printf("thread: %d executes (%d, %d) of process %d  \n",omp_get_thread_num(), x, y, rank);
             	update_cell_static((y == 0 ? top_ghost_row : &local_playground[(y - 1) * xsize]),
                            (y == my_chunk - 1 ? bottom_ghost_row : &local_playground[(y + 1) * xsize]),
                            local_playground, updated_playground, xsize, my_chunk, x, y);
            }
         }
 	memcpy(local_playground, updated_playground, local_size * sizeof(unsigned char));
        free(updated_playground);
        free(top_ghost_row);
        free(bottom_ghost_row);
         if(step % s == 0){
             MPI_Gatherv(local_playground, local_size, MPI_UNSIGNED_CHAR, playground, 
                  sendcounts, displs, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

             write_snapshot(playground, 255, xsize, ysize, "ssnapshot", step);
            }

   }
 if(s != n){
   MPI_Gatherv(local_playground, local_size, MPI_UNSIGNED_CHAR, playground, 
sendcounts, displs, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
 }

    if (rank == 0)
    {
        free(sendcounts);
        free(displs);
    }

    free(local_playground);

}


