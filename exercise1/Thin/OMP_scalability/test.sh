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
make par location=$loc
cd $loc

processes=2


datafile=$loc/timing.csv
echo "threads_per_socket, ordered_mean, static_mean" > $datafile


th_socket=12
	export OMP_NUM_THREADS=$th_socket
	echo -n "${th_socket}," >> $datafile
	mpirun -np $processes --map-by socket par_main.x -r -f "gol4.pgm" -k 8 -e 0 -n 2 -s 0 
	mpirun -np $processes --map-by socket par_main.x -r -f "gol4.pgm" -k 8 -e 1 -n 2 -s 0
done
	

cd ../..
make clean 
module purge
cd $loc
