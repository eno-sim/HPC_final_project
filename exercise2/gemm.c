/*
 * authors   : Giuseppe Piero Brandino - eXact-lab s.r.l.
 * date      : October 2018
 * copyright : GNU Public License
 * modifiedby: Stefano Cozzini for DSSC usage
 */

#define min(x,y) (((x) < (y)) ? (x) : (y))

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef MKL 
#include "mkl_cblas.h"
#endif

#ifdef OPENBLAS
#include "cblas.h"
#endif

#ifdef BLIS
#include "cblas.h"
#endif

#include <time.h>
#include <unistd.h>

#ifdef USE_FLOAT
#define MYFLOAT float
#define DATATYPE printf(" Using float \n\n");
#define GEMMCPU cblas_sgemm
#ifdef MKL
#define RESULTS  "mkl_float.csv"
#endif
#ifdef OPENBLAS
#define RESULTS "openblas_float.csv"
#endif
#ifdef BLIS
#define RESULTS "blis_float.csv"
#endif
#endif

#ifdef USE_DOUBLE
#define MYFLOAT double 
#define DATATYPE printf(" Using double \n\n");
#define GEMMCPU cblas_dgemm
#ifdef MKL
#define RESULTS  "mkl_double.csv"
#endif
#ifdef OPENBLAS
#define RESULTS "openblas_double.csv"
#endif
#ifdef BLIS
#define RESULTS "blis_double.csv"
#endif
#endif

struct timespec diff(struct timespec start, struct timespec end)
{
        struct timespec temp;
        if ((end.tv_nsec-start.tv_nsec)<0) {
                temp.tv_sec = end.tv_sec-start.tv_sec-1;
                temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
        } else {
                temp.tv_sec = end.tv_sec-start.tv_sec;
                temp.tv_nsec = end.tv_nsec-start.tv_nsec;
        }
        return temp;
}


int main(int argc, char** argv)
{
    MYFLOAT *A, *B, *C;
    int m, n, k, i, j;
    MYFLOAT alpha, beta;
    struct timespec begin, end;
    double elapsed;
    if (argc == 1)
    {
    m = 2000, k = 200, n = 1000;
    }
    else if (argc == 4)
    {
        m = atoi(argv[1]);
        k = atoi(argv[2]);
        n = atoi(argv[3]);
    }
    else
    {
    printf( "Usage: %s M K N, the corresponding matrices will be  A(M,K) B(K,N) \n", argv[0]); 
    return 0; 
    }


    printf ("\n This example computes real matrix C=alpha*A*B+beta*C using \n"
            " BLAS function dgemm, where A, B, and  C are matrices and \n"
            " alpha and beta are scalars\n\n");


    printf (" Initializing data for matrix multiplication C=A*B for matrix \n"
            " A(%ix%i) and matrix B(%ix%i)\n\n", m, k, k, n);
    DATATYPE
    alpha = 1.0; beta = 0.0;

    A = (MYFLOAT *)malloc( m*k*sizeof( MYFLOAT ));
    B = (MYFLOAT *)malloc( k*n*sizeof( MYFLOAT ));
    C = (MYFLOAT *)malloc( m*n*sizeof( MYFLOAT ));
    if (A == NULL || B == NULL || C == NULL) {
      printf( "\n ERROR: Can't allocate memory for matrices. Aborting... \n\n");
      free(A);
      free(B);
      free(C);
      return 1;
    }

    for (i = 0; i < (m*k); i++) {
        A[i] = (MYFLOAT)(i+1);
    }

    for (i = 0; i < (k*n); i++) {
        B[i] = (MYFLOAT)(-i-1);
    }

    for (i = 0; i < (m*n); i++) {
        C[i] = 0.0;
    }

    sleep(1);
    printf (" Computing matrix product using gemm function via CBLAS interface \n");

   #ifdef MULTIPLE_ITERATIONS
    double elapsed_sum = 0.0, elapsed_sq_sum = 0.0;
    double gflops_sum = 0.0, gflops_sq_sum = 0.0;
    int num_trials = 30;
    for (int trial=0; trial < num_trials; trial++){
   	 clock_gettime(CLOCK_MONOTONIC, &begin);
   	 GEMMCPU(CblasColMajor, CblasNoTrans, CblasNoTrans,
                m, n, k, alpha, A, m, B, k, beta, C, m);
   	 clock_gettime(CLOCK_MONOTONIC, &end);
  	 elapsed = (double)diff(begin,end).tv_sec + (double)diff(begin,end).tv_nsec / 1000000000.0;
  	 double gflops = 2.0 * m *n*k;
   	 gflops = gflops/elapsed*1.0e-9; 
	 elapsed_sum += elapsed;
         elapsed_sq_sum += elapsed * elapsed;
         gflops_sum += gflops;
         gflops_sq_sum += gflops * gflops;
    }
    double elapsed_mean = elapsed_sum / num_trials;
    double elapsed_sd = sqrt((elapsed_sq_sum / num_trials) - (elapsed_mean * elapsed_mean));
    double gflops_mean = gflops_sum / num_trials;
    double gflops_sd = sqrt((gflops_sq_sum / num_trials) - (gflops_mean * gflops_mean));
    #ifdef WRITE_CSV
      FILE* results;
      results = fopen(RESULTS, "a");
      fprintf(results, "%lf,%lf,%lf,%lf\n", elapsed_mean, elapsed_sd, gflops_mean, gflops_sd);
      fclose(results);
    #else 
      printf("\n Elapsed time mean: %lf s, standard deviation: %lf s\n", elapsed_mean, elapsed_sd);
      printf("%dx%dx%d\t%lf GFLOPS mean, %lf GFLOPS standard deviation\n", m, n, k, gflops_mean, gflops_sd);
    #endif
   #else
    clock_gettime(CLOCK_MONOTONIC, &begin);
    GEMMCPU(CblasColMajor, CblasNoTrans, CblasNoTrans,
                m, n, k, alpha, A, m, B, k, beta, C, m);
    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed = (double)diff(begin,end).tv_sec + (double)diff(begin,end).tv_nsec / 1000000000.0;
    double gflops = 2.0 * m *n*k;
    gflops = gflops/elapsed*1.0e-9;
   #ifdef WRITE_CSV
    FILE* results;
    results = fopen(RESULTS, "a"); 
    fprintf(results, "%lf,%lf\n",  elapsed, gflops);
    fclose(results);
   #else 
    printf ("\n Elapsed time %d.%d s\n\n\n", diff(begin,end).tv_sec, diff(begin,end).tv_nsec );
    printf("%dx%d\t%lf s\t%lf GFLOPS\n", n, k, elapsed, gflops);
   #endif
  #endif

#ifdef PRINT
    printf (" Top left corner of matrix A: \n");
    for (i=0; i<min(m,6); i++) {
      for (j=0; j<min(k,6); j++) {
        printf ("%12.0f", A[j+i*k]);
      }
      printf ("\n");
    }

    printf ("\n Top left corner of matrix B: \n");
    for (i=0; i<min(k,6); i++) {
      for (j=0; j<min(n,6); j++) {
        printf ("%12.0f", B[j+i*n]);
      }
      printf ("\n");
    }
    
    printf ("\n Top left corner of matrix C: \n");
    for (i=0; i<min(m,6); i++) {
      for (j=0; j<min(n,6); j++) {
        printf ("%12.5G", C[j+i*n]);
      }
      printf ("\n");
    }
#endif
    free(A);
    free(B);
    free(C);

    return 0;
}
