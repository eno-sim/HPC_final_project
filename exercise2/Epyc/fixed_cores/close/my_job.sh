#!/bin/bash
#SBATCH --no-requeue
#SBATCH --job-name="ex2"
#SBATCH -n 64
#SBATCH -N 1
#SBATCH --get-user-env
#SBATCH --partition=EPYC
#SBATCH --exclusive
#SBATCH --time=02:30:00

module load architecture/AMD
module load mkl
module load openBLAS/0.3.21-omp
export LD_LIBRARY_PATH=/u/dssc/scappi00/myblis/lib:$LD_LIBRARY_PATH

make clean
make cpu

export OMP_PLACES=cores
export OMP_PROC_BIND=close
# export OMP_PROC_BIND=spread

echo "size, time_mean(s), time_sd, GFLOPS_mean, GFLOPS_sd" >>
for i in {1..18}
do
	let size=$((2000+1000*$i))
	./openblas_double.x $size $size $size >> openblas_double.csv
	./mkl_double.x $size $size $size >> mkl_double.csv
	./blis_double.x $size $size $size >> blis_double.csv
	./openblas_float.x $size $size $size >> openblas_float.csv
	./mkl_float.x $size $size $size >> mkl_float.csv
	./blis_float.x $size $size $size >> blis_float.csv
done
