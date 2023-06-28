#!/bin/bash
#SBATCH --no-requeue
#SBATCH --job-name="MPI_scalaility"
#SBATCH --partition=EPYC
#SBATCH -N 2
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

n=5
threads=12


datafile=$loc/timing.csv
echo "size, ordered_mean, ordered_sd, static_mean, static_sd" > $datafile


## initialize a playground
export OMP_NUM_THREADS=$threads

for size in $(seq 5000 5000 20000)
do
	mpirun -np 4 -N 2 --map-by socket main.x -i  -k $size
	for procs in $(seq 1 1 4)
	do
	  echo -n "${size}," >> $datafile
	  echo -n "${procs},">> $datafile
	  mpirun -np $procs -N 2 --map-by socket main.x -r -e 0 -n $n -s 0
	  mpirun -np $procs -N 2 --map-by socket main.x -r -e 1 -n $n -s 0
	done
done



cd ../..
make clean 
module purge
cd $loc
