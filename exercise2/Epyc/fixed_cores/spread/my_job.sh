#!/bin/bash
#SBATCH --no-requeue
#SBATCH --job-name="ex2"
#SBATCH -n 64
#SBATCH -N 1
#SBATCH --get-user-env
#SBATCH --partition=EPYC
#SBATCH --exclusive
#SBATCH --time=01:30:00

module load architecture/AMD
module load mkl
module load openBLAS/0.3.21-omp
export LD_LIBRARY_PATH=/u/dssc/scappi00/myblis/lib:$LD_LIBRARY_PATH

location=$(pwd)

cd ../../..
make cpu loc=$location


cd $location
policy=spread
arch=EPYC #architecture

export OMP_PLACES=cores
export OMP_PROC_BIND=$policy
# export OMP_PROC_BIND=spread


for lib in openblas mkl blis; do
  for prec in float double; do
    file="${lib}.${prec}.csv"
    echo "arch:,${arch},,," > $file
    echo "library:,${lib^^},,," >> $file
    echo "precision:,${prec},,," >> $file
    echo "policy:,${policy},,," >> $file
    echo ",,,," >> $file
    echo "matrix_size,time_mean(s),time_sd,GFLOPS_mean,GFLOPS_sd" >> $file
  done
done

for i in {1..18}; do
  let size=$((2000+1000*$i))
  for lib in openblas mkl blis; do
    for prec in float double; do
      ./${lib}_${prec}.x $size $size $size
    done
  done
done

cd ../../..
make clean loc=$location
module purge

