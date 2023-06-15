## MKL libraries
###
###
MKL= -L${MKLROOT}/lib/intel64  -lmkl_intel_lp64 -lmkl_gnu_thread -lmkl_core -lgomp -lpthread -lm -ldl

### OpenBLAS libraries 
OPENBLASROOT=${OPENBLAS_ROOT}
### BLIS library
BLISROOT=/u/dssc/scappi00/myblis

cpu: mkl_double.x mkl_float.x openblas_double.x openblas_float.x blis_double.x blis_float.x


mkl_double.x: gemm.c
	gcc -DUSE_DOUBLE -DMKL $^ -m64 -I${MKLROOT}/include $(MKL)  -o $@ -DWRITE_CSV -DMULTIPLE_ITERATIONS

openblas_double.x: gemm.c
	gcc -DUSE_DOUBLE -DOPENBLAS $^ -m64 -I${OPENBLASROOT}/include -L/${OPENBLASROOT}/lib -lopenblas -lpthread -o $@ -fopenmp -DWRITE_CSV -DMULTIPLE_ITERATIONS

blis_double.x: gemm.c
	gcc -DUSE_DOUBLE  -DBLIS $^ -m64 -I${BLISROOT}/include/blis -L/${BLISROOT}/lib -o $@ -lpthread  -lblis -fopenmp -lm -DWRITE_CSV -DMULTIPLE_ITERATIONS

mkl_float.x: gemm.c
	gcc -DUSE_FLOAT -DMKL $^ -m64 -I${MKLROOT}/include $(MKL) $@ -DWRITE_CSV -DMULTIPLE_ITERATIONS


openblas_float.x: gemm.c
        gcc -DUSE_FLOAT -DOPENBLAS $^ -m64 -I${OPENBLASROOT}/include -L/${OPENBLASROOT}/lib -lopenblas -lpthread -o $@ -fopenmp -DWRITE_CSV -DMULTIPLE_ITERATIONS


blis_float.x: gemm.c
       gcc -DUSE_FLOAT  -DBLIS $^ -m64 -I${BLISROOT}/include/blis -L/${BLISROOT}/lib -o $@ -lpthread  -lblis -fopenmp -lm -DWRITE_CSV -DMULTIPLE_ITERATIONS


clean:
	rm -rf *.x
