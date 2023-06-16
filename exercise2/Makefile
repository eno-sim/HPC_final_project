## MKL libraries
###
###
MKL= -L${MKLROOT}/lib/intel64  -lmkl_intel_lp64 -lmkl_gnu_thread -lmkl_core -lgomp -lpthread -lm -ldl

### OpenBLAS libraries 
OPENBLASROOT=${OPENBLAS_ROOT}
### BLIS library
BLISROOT=/u/dssc/scappi00/myblis

cpu: ${loc}/mkl_double.x ${loc}/mkl_float.x ${loc}/openblas_double.x ${loc}/openblas_float.x ${loc}/blis_double.x ${loc}/blis_float.x


${loc}/mkl_double.x: gemm.c
	gcc -DUSE_DOUBLE -DMKL $^ -m64 -I${MKLROOT}/include $(MKL)  -o $@ -DWRITE_CSV -DMULTIPLE_ITERATIONS

${loc}/openblas_double.x: gemm.c
	gcc -DUSE_DOUBLE -DOPENBLAS $^ -m64 -I${OPENBLASROOT}/include -L/${OPENBLASROOT}/lib -lopenblas -lpthread -o $@ -fopenmp -lm -DWRITE_CSV -DMULTIPLE_ITERATIONS

${loc}/blis_double.x: gemm.c
	gcc -DUSE_DOUBLE  -DBLIS $^ -m64 -I${BLISROOT}/include/blis -L/${BLISROOT}/lib -o $@ -lpthread  -lblis -fopenmp -lm -DWRITE_CSV -DMULTIPLE_ITERATIONS

${loc}/mkl_float.x: gemm.c
	gcc -DUSE_FLOAT -DMKL $^ -m64 -I${MKLROOT}/include $(MKL) -o $@ -DWRITE_CSV -DMULTIPLE_ITERATIONS


${loc}/openblas_float.x: gemm.c
	gcc -DUSE_FLOAT -DOPENBLAS $^ -m64 -I${OPENBLASROOT}/include -L/${OPENBLASROOT}/lib -lopenblas -lpthread -o $@ -fopenmp -lm -DWRITE_CSV -DMULTIPLE_ITERATIONS


${loc}/blis_float.x: gemm.c
	gcc -DUSE_FLOAT  -DBLIS $^ -m64 -I${BLISROOT}/include/blis -L/${BLISROOT}/lib -o $@ -lpthread  -lblis -fopenmp -lm -DWRITE_CSV -DMULTIPLE_ITERATIONS


clean:
	rm -rf ${loc}/*.x
