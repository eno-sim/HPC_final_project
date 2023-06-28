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
    if(e == ORDERED){
       int num = MAXVAL;
       unsigned char *playground_o = (unsigned char *)calloc(k * k,  sizeof(unsigned char));
       read_pgm_image((void **)&playground_o, &num, &k, &k, fname);
         
       if(s>0){  
         for (int i = 1; i <= n; i += s)
            {
                ordered_evolution(playground_o, k, k, s);
                write_snapshot(playground_o, MAXVAL, k, k, "osnapshot", i);
            }
         free(playground_o);
       }
       else if (s==0){
          double *times = (double *)calloc(n, sizeof(double));

        double start_time = omp_get_wtime();
        for (int i = 1; i <= n; i += 1){
            ordered_evolution(playground_o, k, k, 1);
            times[i - 1] = omp_get_wtime();
        }
        write_snapshot(playground_o, MAXVAL, k, k, "osnapshot", n);
        free(playground_o);

        // Compute the time taken for each iteration
        for (int i = 0; i < n; i++) {
            times[i] = (i == 0 ? times[i]-start_time : times[i] - times[i - 1]);
        }
        // Compute the mean time per iteration
        double mean = 0.0;
        for (int i = 0; i < n; i++) {
            mean += times[i];
        }
        mean /= n;

        // Compute the standard deviation of the time per iteration
        double std_dev = 0.0;
        for (int i = 0; i < n; i++) {
            std_dev += ((times[i] - mean)*(times[i] - mean));
        }
        std_dev = sqrt(std_dev / n);

        // Write the mean and standard deviation to the CSV file
        FILE *fp = fopen("timing.csv", "a");
        fprintf(fp, "%f,%f,", mean, std_dev);
        fclose(fp);
       }}

    else if(e == STATIC){
      int num = MAXVAL;
      unsigned char * playground_s = (unsigned char *)malloc(k*k*sizeof(unsigned char));
      read_pgm_image((void **)&playground_s, &num, &k, &k, fname);



      if(s > 0) {
         for (int i = 1; i <= n; i += s)
            {
                static_evolution(playground_s, k, k, s);
                write_snapshot(playground_s, 255, k, k, "ssnapshot", i);
            }
            free(playground_s);
        }

      
      else if(s==0){
         double *times = (double *)calloc(n, sizeof(double));
  
         double start_time = omp_get_wtime();
         for (int i = 1; i <= n; i += 1)
            {
                static_evolution(playground_s, k, k, 1);
                times[i - 1] = omp_get_wtime();
            }
            write_snapshot(playground_s, 255, k, k, "ssnapshot", n);
            free(playground_s);
           // Compute the time taken for each iteration
        for (int i = 0; i < n; i++) {
            times[i] = (i == 0 ? times[i]-start_time : times[i] - times[i - 1]);
        }
        // Compute the mean time per iteration
        double mean = 0.0;
        for (int i = 0; i < n; i++) {
            mean += times[i];
        }
        mean /= n;
         // Compute the standard deviation of the time per iteration
        double std_dev = 0.0;
        for (int i = 0; i < n; i++) {
            std_dev += ((times[i] - mean)*(times[i] - mean));
        }
        std_dev = sqrt(std_dev / n);
         // Write the mean and standard deviation to the CSV file
        FILE *fp = fopen("timing.csv", "a");
        fprintf(fp, "%f,%f\n", mean, std_dev);
        fclose(fp);
        
        
        }


    
    else {
      printf("Error!");
    }
  }
  

  if ( fname != NULL ){
    free ( fname );
    
  return 0; }
  }
  }


// counts number of neighbours and updates the state of a single cell


/*
Next steps: evaluate the correctness of the code so far
;  write down the algorithm used so far; look for improvement;
test the code on ORFEO.
*/



