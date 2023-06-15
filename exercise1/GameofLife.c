#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include "mpi.h"
#include <omp.h>
#include <getopt.h>
//Implement utility functions:
//You'll need to implement utility functions such as init_playground,
// load_playground, save_playground,
// update_cell, ordered_evolution, and static_evolution.
 



#define XWIDTH 256
#define YWIDTH 256
#define MAXVAL 255


#if ((0x100 & 0xf) == 0x0)
#define I_M_LITTLE_ENDIAN 1
#define swap(mem) (( (mem) & (short int)0xff00) >> 8) +	\
  ( ((mem) & (short int)0x00ff) << 8)
#else
#define I_M_LITTLE_ENDIAN 0
#define swap(mem) (mem)
#endif

void write_pgm_image( void *image, int maxval, int xsize, int ysize, const char *image_name)
/*
 * image        : a pointer to the memory region that contains the image
 * maxval       : either 255 or 65536
 * xsize, ysize : x and y dimensions of the image
 * image_name   : the name of the file to be written
 *
 */
{
  FILE* image_file; 
  image_file = fopen(image_name, "w"); 
  
  // Writing header
  // The header's format is as follows, all in ASCII.
  // "whitespace" is either a blank or a TAB or a CF or a LF
  // - The Magic Number (see below the magic numbers)
  // - the image's width
  // - the height
  // - a white space
  // - the image's height
  // - a whitespace
  // - the maximum color value, which must be between 0 and 65535
  //
  // if he maximum color value is in the range [0-255], then
  // a pixel will be expressed by a single byte; if the maximum is
  // larger than 255, then 2 bytes will be needed for each pixel
  //

  int color_depth = 1 + ( maxval > 255 );

  fprintf(image_file, "P5\n# generated by\n# put here your name\n%d %d\n%d\n", xsize, ysize, maxval);
  
  // Writing file
  fwrite( image, 1, xsize*ysize*color_depth, image_file);  

  fclose(image_file); 
  return ;

}


void read_pgm_image( void **image, int *maxval, int *xsize, int *ysize, const char *image_name)
/*
 * image        : a pointer to the pointer that will contain the image
 * maxval       : a pointer to the int that will store the maximum intensity in the image
 * xsize, ysize : pointers to the x and y sizes
 * image_name   : the name of the file to be read
 *
 */
{
  FILE* image_file; 
  image_file = fopen(image_name, "r"); 

  *image = NULL;
  *xsize = *ysize = *maxval = 0;
  
  char    MagicN[2];
  char   *line = NULL;
  size_t  k, n = 0;
  
  // get the Magic Number
  k = fscanf(image_file, "%2s%*c", MagicN );

  // skip all the comments
  k = getline( &line, &n, image_file);
  while ( (k > 0) && (line[0]=='#') )
    k = getline( &line, &n, image_file);

  if (k > 0)
    {
      k = sscanf(line, "%d%*c%d%*c%d%*c", xsize, ysize, maxval);
      if ( k < 3 )
	      fscanf(image_file, "%d%*c", maxval);
    }
  else
    {
      *maxval = -1;         // this is the signal that there was an I/O error
			    // while reading the image header
      free( line );
      return;
    }
  free( line );
  
  int color_depth = 1 + ( *maxval > 255 );
  unsigned int size = *xsize * *ysize * color_depth;
  
  if ( (*image = (char*)malloc( size )) == NULL )
    {
      fclose(image_file);
      *maxval = -2;         // this is the signal that memory was insufficient
      *xsize  = 0;
      *ysize  = 0;
      return;
    }
  
  if ( fread( *image, 1, size, image_file) != size )
    {
      free( image );
      image   = NULL;
      *maxval = -3;         // this is the signal that there was an i/o error
      *xsize  = 0;
      *ysize  = 0;
    }  

  fclose(image_file);
  return;
}


void swap_image( void *image, int xsize, int ysize, int maxval )
/*
 * This routine swaps the endianism of the memory area pointed
 * to by ptr, by blocks of 2 bytes
 *
 */
{
  if ( maxval > 255 )
    {
      // pgm files has the short int written in
      // big endian;
      // here we swap the content of the image from
      // one to another
      //
      unsigned int size = xsize * ysize;
      for ( int i = 0; i < size; i++ )
  	((unsigned short int*)image)[i] = swap(((unsigned short int*)image)[i]);
    }
  return;
}


void * init_playground(int xsize, int ysize)
{
    srand(time(NULL));

    // Allocate memory for the image
    unsigned char *image = (unsigned char *)malloc(xsize * ysize * sizeof(unsigned char));

    // Generate random black-and-white pixel values
    for (int y = 0; y < ysize; y++)
    {
        for (int x = 0; x < xsize; x++)
        {
            image[y * xsize + x] = (rand() % 2) * MAXVAL;
        }
    }

    // Return the pointer to the allocated memory
    return (void *)image;
}

void write_snapshot(unsigned char *playground, int maxval, int xsize, int ysize, const char *basename, int iteration)
{
    char filename[256];
    snprintf(filename, sizeof(filename), "%s_%05d.pgm", basename, iteration);
    write_pgm_image((void *)playground, maxval, xsize, ysize, filename);
}




#define INIT 1
#define RUN  2

#define K_DFLT 100

#define ORDERED 0
#define STATIC  1


// char fname_deflt[] = "game_of_life.pgm";

int   action = 0;
int   k      = K_DFLT;  //size of the squared  playground
int   e      = ORDERED; //evolution type [0\1]
int   n      = 10000;  // number of iterations 
int   s      = 1;      // every how many steps a dump of the system is saved on a file
                      // 0 meaning only at the end.
char *fname  = NULL;  // name of the file to be either read or written


int main ( int argc, char **argv )
{
  /*Each character in the optstring represents a single-character
    option that the program accepts. If a character is followed by a colon (:),
    it indicates that the option requires an argument.
     -i: No argument required. Initialize playground.
     -r: No argument required. Run a playground.
     -k: Requires an argument (e.g., -k 100). Playground size.
     -e: Requires an argument (e.g., -e 1). Evolution type.
     -f: Requires an argument (e.g., -f filename.pgm). 
     Name of the file to be either read or written
     -n: Requires an argument (e.g., -n 10000). Number of steps.
     -s: Requires an argument (e.g., -s 1). Frequency of dump.*/
  char *optstring = "irk:e:f:n:s:";

  int c;
  /*When the getopt function is called in the while loop,
    it processes the command-line arguments according to
    the format specified in optstring.
    The getopt function returns the next option character or -1 if there are no more options
 */
  while ((c = getopt(argc, argv, optstring)) != -1) {
    switch(c) {
      
    case 'i':
      action = INIT; 

      
      break;
      
    case 'r':
      action = RUN; break;
      
    case 'k':
      k = atoi(optarg); break;

    case 'e':
      e = atoi(optarg); break;

    case 'f':
      fname = (char*)malloc( sizeof(optarg)+1 );
      sprintf(fname, "%s", optarg );
      break;  

    case 'n':
      n = atoi(optarg); break;

    case 's':
      s = atoi(optarg); break;

    default :
      printf("argument -%c not known\n", c ); break;
    }
  }

  if(action==INIT){
    void * ptr = init_playground(k, k);
    write_pgm_image(ptr, 255, k, k, fname);
  }

  if (action==RUN)
  {
    if(e == ORDERED){
       unsigned char *playground_o = (unsigned char *)calloc(k * k * sizeof(unsigned char));
       read_pgm_image((void **)&playground_o, &MAXVAL, &k, &k, fname);
         for (int i = 0; i <= n; i += s)
            {
                ordered_evolution(playground_o, k, k, s);
                write_snapshot(playground_o, MAXVAL, k, k, "snapshot", i);
            }
       free(playground_o);

    }

    else if(e == STATIC){
      unsigned char * playground_s = (unsigned char *)calloc(k*k*sizeof(unsigned char));
       read_pgm_image((void **)&playground_s, &MAXVAL, &k, &k, fname);
         for (int i = 0; i <= n; i += s)
            {
                static_evolution(playground_s, k, k, s);
                write_snapshot(playground_s, 255, k, k, "snapshot", i);
            }
            free(playground_s);
        }
    
    else {
      printf("Error!");
    }
  }
  

  if ( fname != NULL )
    free ( fname );
    
  return 0;
}


// counts number of neighbours and updates the state of a single cell
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

void ordered_evolution(unsigned char *playground, int xsize, int ysize, int n)
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

            if (ny == -1)
            {
                cell_value = top_row[nx];
            }
            else if (ny == ysize)
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
     At the end of the computation, the root process will gather 
    the results from all other processes using MPI_Gatherv, 
    reconstructing the whole playground with the updated cell states.

*/


void static_evolution(unsigned char *playground, int xsize, int ysize, int n)
{
    int rank, size;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); //get the rank of the current process
    MPI_Comm_size(MPI_COMM_WORLD, &size); //get the total number of processes

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

    if (rank == 0)
    {
        free(sendcounts);
        free(displs);
    }

    for (int step = 0; step < n; step++)
    {
        unsigned char *top_ghost_row = (unsigned char *)malloc(xsize * sizeof(unsigned char));
        unsigned char *bottom_ghost_row = (unsigned char *)malloc(xsize * sizeof(unsigned char));


       unsigned char *updated_playground = (unsigned char *)malloc(local_size * sizeof(unsigned char));


        int top_neighbor = (rank - 1 + size) % size;
        int bottom_neighbor = (rank + 1) % size;
        
        
        
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
        {
        for (int y = 0; y < my_chunk; y++)
         {
            for (int x = 0; x < xsize; x++)
            {
                update_cell_static((y == 0 ? top_ghost_row : &local_playground[(y - 1) * xsize]),
                            (y == my_chunk - 1 ? bottom_ghost_row : &local_playground[(y + 1) * xsize]),
                            local_playground, updated_playground, xsize, my_chunk, x, y);
            }
         }
        }
        memcpy(local_playground, updated_playground, local_size * sizeof(unsigned char));
        free(updated_playground);
        free(top_ghost_row);
        free(bottom_ghost_row);
   }

    MPI_Gatherv(local_playground, local_size, MPI_UNSIGNED_CHAR, playground, 
    sendcounts, displs, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

    free(local_playground);

    MPI_Finalize();
}



/*
Next steps: evaluate the correctness of the code so far
;  write down the algorithm used so far; look for improvement;
test the code on ORFEO.
*/


