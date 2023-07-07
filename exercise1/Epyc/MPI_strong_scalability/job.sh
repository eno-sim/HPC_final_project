#!/bin/bash
#SBATCH --no-requeue
#SBATCH --job-name="strong_mpi"
#SBATCH --partition=EPYC
#SBATCH -N 2
#SBATCH -n 256
#SBATCH --exclusive
#SBATCH --time=02:00:00

module load openMPI/4.1.5/gnu/12.2.1 
policy=close
export OMP_PLACES=cores
export OMP_PROC_BIND=$policy

loc=$(pwd)
cd ../..
make par location=$loc
cd $loc

threads=1


datafile=$loc/timing.csv
#echo "size, procs, ordered_mean, static_mean" > $datafile
#echo "size, procs, static_mean" > $datafile

## initialize a playground
export OMP_NUM_THREADS=$threads
size=40000
#for size in 20000 30000
#do
mpirun -np 4 -N 2 --map-by socket par_main.x -i -f "playground_${size}.pgm" -k $size
for procs in $(seq 216 8 256)
do
	  echo -n "${size}," >> $datafile
	  echo -n "${procs},">> $datafile
#	  mpirun -np $procs -N 2 --map-by core par_main.x -r -f "playground_${size}.pgm" -e 0 -n 3 -s 0 -k $size
	  mpirun -np $procs -N 2 --map-by core par_main.x -r -f "playground_${size}.pgm" -e 1 -n 100 -s 0 -k $size
done
#done



cd ../..
make clean 
module purge
cd $loc
