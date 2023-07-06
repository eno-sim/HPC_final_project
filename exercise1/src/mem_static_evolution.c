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




void static_evolution(unsigned char *local_playground, int xsize, int my_chunk, int my_offset,  int n, int s)
{
    int rank, size;
   
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); //get the rank of the current process
    MPI_Comm_size(MPI_COMM_WORLD, &size); //get the total number of processes


    int local_size = my_chunk * xsize * sizeof(unsigned char);    
    


     unsigned char *top_ghost_row = (unsigned char *)malloc(xsize * sizeof(unsigned char));
     unsigned char *bottom_ghost_row = (unsigned char *)malloc(xsize * sizeof(unsigned char));


     int top_neighbor = (rank - 1 + size) % size; // always rank-1 except for when rank==0, where
        // the result of the modulus is size-1 (the last process in rank order is the top neighbor of process 0)

     int bottom_neighbor = (rank + 1) % size; // (rank + 1) / size is always 0, so the modulus will always
        //be rank+1, except for when rank == size-1 (maximum) where the modulus is 0 (process 0 is the bottom 
        //neighbor of the last process)




    for (int step = 1; step <= n; step++)
    {
	unsigned char *updated_playground = (unsigned char *)malloc(local_size);
       MPI_Request request[2];

	// Each process sends its top row to its top neighbor
	MPI_Isend(&local_playground[0], xsize, MPI_UNSIGNED_CHAR, top_neighbor, 0, 
          MPI_COMM_WORLD, &request[0]);

	// Each process sends its bottom row to its bottom neighbor
	MPI_Isend(&local_playground[(my_chunk - 1) * xsize], xsize, MPI_UNSIGNED_CHAR, bottom_neighbor, 1,
          MPI_COMM_WORLD, &request[1]);

	// Each process receives its bottom ghost row from its bottom neighbor
	MPI_Recv(bottom_ghost_row, xsize, MPI_UNSIGNED_CHAR, bottom_neighbor, 0, 
         MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	// Each process receives its top ghost row from its top neighbor
	MPI_Recv(top_ghost_row, xsize, MPI_UNSIGNED_CHAR, top_neighbor, 
         1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
       
       
       #pragma omp parallel for collapse(2) 
       for (int y = 0; y < my_chunk; y++)
         {
            for (int x = 0; x < xsize; x++)
            {   
             	update_cell_static((y == 0 ? top_ghost_row : &local_playground[(y - 1) * xsize]),
                            (y == my_chunk - 1 ? bottom_ghost_row : &local_playground[(y + 1) * xsize]),
                            local_playground, updated_playground, xsize, my_chunk, x, y);
            }
         }
 	
//        memcpy(local_playground, updated_playground, local_size * sizeof(unsigned char));
	unsigned char *temp = local_playground;
	local_playground = updated_playground;
	updated_playground = temp;
 	free(updated_playground); 
     
        if(step % s == 0){
             par_write_snapshot(local_playground, 255, xsize, my_chunk, "ssnapshot", step, my_offset); //CHECK IT!!!!
            	 }

   }

 free(top_ghost_row);
 free(bottom_ghost_row);





 if(s != n){
   par_write_snapshot(local_playground, 255, xsize, my_chunk, "ssnapshot", n, my_offset);
 
}



}


