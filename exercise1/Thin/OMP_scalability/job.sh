#!/bin/bash
#SBATCH --no-requeue
#SBATCH --job-name="openMP_scal"
#SBATCH --partition=THIN
#SBATCH -N 1
#SBATCH -n 24
#SBATCH --exclusive
#SBATCH --time=02:00:00


module load openMPI/4.1.5/gnu/12.2.1 
policy=close
export OMP_PLACES=cores
export OMP_PROC_BIND=$policy

loc=$(pwd)
cd ../..
make all location=$loc
cd $loc

processes=2


datafile=$loc/timing.csv
echo "threads_per_socket, ordered_mean, static_mean" > $datafile



for th_socket in $(seq 1 1 12)
do
	export OMP_NUM_THREADS=$th_socket
	echo -n "${th_socket}," >> $datafile
	mpirun -np 1 --map-by socket main.x -r -f "gol4.pgm" -e 0 -n 1 -s 0 -k 8
	mpirun -np $processes --map-by socket main.x -r -f "gol4.pgm" -e 1 -n 1 -s 0 -k 8
done
	

cd ../..
make clean 
module purge
cd $loc
