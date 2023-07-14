#!/bin/bash
#SBATCH --no-requeue
#SBATCH --job-name="openMP_scal"
#SBATCH --partition=EPYC
#SBATCH -N 1
#SBATCH -n 128 
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

processes=2
size=25000

datafile=$loc/timing.csv
#echo "threads_per_socket, ordered_mean, static_mean" > $datafile

mpirun main.x -i -k $size -f "playground.pgm"


for th_socket in 1 $(seq 2 2 64)
do
	export OMP_NUM_THREADS=$th_socket
	echo -n "${th_socket}," >> $datafile
	mpirun -np $processes --map-by socket main.x -r -f "playground.pgm" -e 0 -n 3 -s 0 -k $size
	mpirun -np $processes --map-by socket main.x -r -f "playground.pgm" -e 1 -n 50 -s 0 -k $size
done


cd ../..
make clean 
module purge
cd $loc
