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
make all location=$loc
cd $loc

xsize=20000
ysize=20000
n=5
processes=2


datafile=$loc/timing.csv
echo "threads_per_socket, ordered_mean, static_mean" > $datafile

mpirun -np $processes --map-by socket main.x -i -f "game_of_life_20000.pgm" -k $ysize

for th_socket in $(seq 1 1 64)
do
	export OMP_NUM_THREADS=$th_socket
	echo -n "${th_socket}," >> $datafile
	mpirun -np $processes --map-by socket main.x -r -f "game_of_life_20000.pgm" -e 0 -n $n -s 0 
	mpirun -np $processes --map-by socket main.x -r -f "game_of_life_20000.pgm" -e 1 -n $n -s 0 
done
	

cd ../..
make clean 
module purge
cd $loc
