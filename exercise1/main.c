#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include "mpi.h"
#include <omp.h>
#include <getopt.h>
#include <math.h>
#include "ordered_evolution.h"
#include "read_write_pgm.h"
#include "static_evolution.h"

//Implement utility functions:
//You'll need to implement utility functions such as init_playground,
// load_playground, save_playground,
// update_cell, ordered_evolution, and static_evolution.
 



#define XWIDTH 256
#define YWIDTH 256
#define MAXVAL 255
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


int main ( int argc, char **argv ) {
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
    double start_time;
    double end_time;
    
    if(e == ORDERED){
       int num = MAXVAL;
       unsigned char *playground_o = (unsigned char *)calloc(k * k,  sizeof(unsigned char));
       read_pgm_image((void **)&playground_o, &num, &k, &k, fname);
         
       if(s>0){
         start_time = omp_get_wtime();  
         for (int i = 1; i <= n; i += s)
            {
                ordered_evolution(playground_o, k, k, s);
                write_snapshot(playground_o, MAXVAL, k, k, "osnapshot", i);
            }
           end_time = omp_get_wtime();
           time_elapsed = end_time - start_time;
           mean_time = time_elapsed / n;
      
         free(playground_o);
       }

       else if (s==0){
        start_time = omp_get_wtime();
        for (int i = 1; i <= n; i += 1){
            ordered_evolution(playground_o, k, k, 1);
        }
        write_snapshot(playground_o, MAXVAL, k, k, "osnapshot", n);
         end_time = omp_get_wtime();
         time_elapsed = end_time - start_time;
         mean_time = time_elapsed / n;
     
        free(playground_o);
        // Write the mean and standard deviation to the CSV file
        FILE *fp = fopen("timing.csv", "a");
        fprintf(fp, "%f,", mean_time);
        fclose(fp);
       }
    }

   
   
   
    else if(e == STATIC){
      int num = MAXVAL;
      unsigned char * playground_s = (unsigned char *)malloc(k*k*sizeof(unsigned char));
      read_pgm_image((void **)&playground_s, &num, &k, &k, fname);



      if(s > 0) {
                start_time = omp_get_wtime();  
                MPI_Init(NULL, NULL);
                static_evolution(playground_s, k, k, n, s);
                MPI_Finalize();
                end_time = omp_get_wtime();
                time_elapsed = end_time - start_time;
                mean_time = time_elapsed / n;
            }
       
     
      else if(s==0){
                start_time = omp_get_wtime();  
                MPI_Init(NULL, NULL);
                static_evolution(playground_s, k, k, n, n);
                MPI_Finalize();
                end_time = omp_get_wtime();
                time_elapsed = end_time - start_time;
                mean_time = time_elapsed / n;
       }
   
      else {
      printf("Error!");
     }
        free(playground_s);
        FILE *fp = fopen("timing.csv", "a");
        fprintf(fp, "%f\n", mean_time);
        fclose(fp);
  }
  
}
  if ( fname != NULL ){
    free ( fname );
  }
  return 0; 

  
}  

