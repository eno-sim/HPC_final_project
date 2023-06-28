#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include "mpi.h"
#include <omp.h>

#define MAXVAL 255









void update_cell_ordered(unsigned char *playground, int xsize, int ysize, int x, int y)
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
            alive_neighbors += (playground[ny * xsize + nx]==MAXVAL);
        }
    }

    int cell_index = y * xsize + x;
    playground[cell_index] = (playground[cell_index] && (alive_neighbors == 2 || alive_neighbors == 3)) || (!playground[cell_index] && alive_neighbors == 3);
}

void ordered_evolution(unsigned char *playground, int xsize, int ysize, int n, int s)
{
    int nthreads;
    int chunk;
    int mod;
    int order;

    #pragma omp parallel
    {
        #pragma omp single
        {
            nthreads = omp_get_num_threads();
            chunk = ysize / nthreads;
            mod = ysize % nthreads;
        }
    }

    for (int step = 0; step < n; step++)
    {
        order = 0;
        #pragma omp parallel
        {
            int me = omp_get_thread_num();
            int done = 0;
            int my_first = chunk * me + ((me < mod) ? me : mod);
            int my_chunk = chunk + (mod > 0) * (me < mod);

            while (!done)
            {
                #pragma omp critical
                {
                    if (order == me)
                    {
                        for (int y = my_first; y < my_first + my_chunk; y++)
                        {
                            for (int x = 0; x < xsize; x++)
                            {
                                update_cell_ordered(playground, xsize, ysize, x, y);
                            }
                        }
                        order++;
                        done = 1;
                    }
                }
            }
        }
    }
}


