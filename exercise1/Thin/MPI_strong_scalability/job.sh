#!/bin/bash
#SBATCH --no-requeue
#SBATCH --job-name="strongmpi_45"
#SBATCH --partition=THIN
#SBATCH -N 2
#SBATCH -n 48
#SBATCH --exclusive
#SBATCH --time=02:00:00
#SBATCH --nodelist=thin[007-008]

module load openMPI/4.1.5/gnu/12.2.1 
policy=close
export OMP_PLACES=cores
export OMP_PROC_BIND=$policy

loc=$(pwd)
cd ../..
make par location=$loc
cd $loc




datafile=$loc/timing.csv
echo "size, procs, ordered_mean, static_mean" > $datafile


## initialize a playground
export OMP_NUM_THREADS=1
size=25000
#for size in 20000 30000
#do
mpirun -np 1 -N 1 --map-by socket par_main.x -i -f "playground_${size}.pgm" -k $size
for procs in 1 $(seq 2 2 48)
do
	  echo -n "${size}," >> $datafile
	  echo -n "${procs},">> $datafile
	  mpirun -np $procs -N 2 --map-by core par_main.x -r -f "playground_${size}.pgm" -e 0 -n 3 -s 0 -k $size
	  mpirun -np $procs -N 2 --map-by core par_main.x -r -f "playground_${size}.pgm" -e 1 -n 50 -s 0 -k $size
done
#done



cd ../..
make clean 
module purge
cd $loc
