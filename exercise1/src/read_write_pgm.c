#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include "mpi.h"
#include <omp.h>
#include <getopt.h>

#define MAXVAL 255

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

  fprintf(image_file, "P5\n# generated by\n# enosim\n%d %d\n%d\n", xsize, ysize, maxval);
  
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
  //"%*c" discards the next character (usually a newline or whitespace).

  // skip all the comments, the purpose of the while is to skip all the 
  // comments.
  k = getline( &line, &n, image_file); // n stores the size 
  // of the read buffer.
  while ( (k > 0) && (line[0]=='#') ) {
    k = getline( &line, &n, image_file);
  }

// this conditions is true when the comments end and we have read the
// first meaningful line of data
  if (k > 0)
    {
      k = sscanf(line, "%d%*c%d%*c%d%*c", xsize, ysize, maxval);
      //If sscanf didn't successfully parse three 
      //items from the line, this line of code attempts 
      //to read an integer from the image_file stream 
      //directly into maxval.
      if ( k < 3 )
	      fscanf(image_file, "%d%*c", maxval);
    }
  else //executed if comments end and there is no content in the file
  // or there was an error
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
    parallel_write_pgm_image((void *)playground, maxval, xsize, ysize, filename);
}


// we can use OpenMP beacuse we are not writing to files 
// but just initializing the matrix as an unsigned char one dimensional vector


unsigned char * parallel_init_playground(int xsize, int ysize)
{
    // Allocate memory for the image
    unsigned char *image = (unsigned char *)malloc(xsize * ysize * sizeof(unsigned char));

    // Use the current time to randomize the seed
    unsigned int time_seed = time(NULL);

    // Generate random black-and-white pixel values
    #pragma omp parallel
    {
        // Each thread has a unique seed
        unsigned int seed = time_seed ^ omp_get_thread_num();

        #pragma omp for schedule(static, 1)
        for(int i = 0; i < xsize * ysize; i++){
            if(rand_r(&seed) % 2){
                image[i] = MAXVAL;
            } else {
                image[i] = 0;
            }
        }
    }

    // Return the pointer to the allocated memory
    return image;
}




void parallel_read_pgm_image(void **image, int *maxval, int *xsize, int *ysize, const char *image_name) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int offset = 0;

    // skipping comments and calculating the initial offset
    // done only by master process
    if (rank == 0) {
        FILE* image_file; 
        image_file = fopen(image_name, "r"); 

        *image = NULL;
        *xsize = *ysize = *maxval = 0;
  
        char MagicN[2];
        char *line = NULL;
        size_t k, n = 0;
        k = fscanf(image_file, "%2s%*c", MagicN );
        offset += k;

        k = getline( &line, &n, image_file);
        offset+=k;

        while ( (k > 0) && (line[0]=='#') ){

            k = getline( &line, &n, image_file);
            offset+=k;
        }
        if (k > 0) {
            k = sscanf(line, "%d%*c%d%*c%d%*c", xsize, ysize, maxval);
            if ( k < 3 ){
            	   k = getline(&line, &n, image_file);
                   offset += k;
                   k = sscanf(line, "%d%*c", maxval);
                }
        }	       
        else {
            *maxval = -1;        
            free( line );
             return;
        }

        free( line ); 
        fclose(image_file);  
    }

    MPI_Bcast(&offset,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(xsize,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(ysize, 1, MPI_INT, 0, MPI_COMM_WORLD);


    int color_depth = 1 + (*maxval > 255);

    // calculating the local image size destined to each process and the offset for each one of them.
    // The initial offset is the same for every process (a sort of relative zero), it takes into account the (in our case 5) first
    // lines of the pgm image before the actual image.
    int chunk = *ysize / size;
    int mod = *ysize % size;
    int my_chunk = chunk + (rank < mod);
    int my_first = rank * chunk + (rank < mod ? rank : mod);
    offset += my_first * (*xsize) * color_depth;

    *image = malloc(my_chunk * (*xsize) * color_depth);

    MPI_File fh;
    MPI_Status status;
    MPI_File_open(MPI_COMM_WORLD, image_name, MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
    MPI_File_seek(fh, offset, MPI_SEEK_SET);
    MPI_File_read(fh, *image, my_chunk * (*xsize) * color_depth, MPI_UNSIGNED_CHAR, &status);
    
    MPI_Barrier(MPI_COMM_WORLD);
    
    MPI_File_close(&fh);
}

void parallel_write_pgm_image(void *image, int maxval, int xsize, int ysize, const char *image_name) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    printf("I am process %d of %d, so far so good \n", rank, size);
    int color_depth = 1 + (maxval > 255);
    int chunk = ysize / size;
    int mod = ysize % size;
    int my_chunk = chunk + (rank < mod);
    int my_first = rank * chunk + (rank < mod ? rank : mod);
    int offset = my_first * xsize * color_depth;
     MPI_File fh;
   
    MPI_Status status;
    int err=0;
    const int header_size = 26;
    if (rank == 0) {
        char header[26];
        sprintf(header, "P5 \n%8d %8d\n%3d\n", xsize, ysize, maxval);
        err = MPI_File_open(MPI_COMM_SELF, image_name, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fh);
        if (err!=0) {
            printf("Error opening file for writing header: %d\n", err);
            return;
        }
        err = MPI_File_write(fh, header, header_size, MPI_CHAR, &status);
        if (err!=0) {
            printf("Error writing header: %d\n", err);
            return;
        }
       MPI_File_close(&fh);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    err = MPI_File_open(MPI_COMM_WORLD, image_name, MPI_MODE_WRONLY, MPI_INFO_NULL, &fh);
        if (err!=0) {
            printf("Error opening file for writing data: %d\n", err);
            return;
        }
    

    offset += header_size;

    

    err += MPI_File_write_at_all(fh, offset,(unsigned char *) image + offset - header_size , my_chunk * xsize * color_depth, MPI_UNSIGNED_CHAR, &status);
    if (err!=0) {
        printf("Error writing data: %d\n", err);
        return;
    }

   MPI_Barrier(MPI_COMM_WORLD);

   err = MPI_File_close(&fh);
    if (err!=0) {
        printf("Error closing file: %d\n", err);
        return;
    }
}




