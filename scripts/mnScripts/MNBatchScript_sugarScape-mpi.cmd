#!/bin/bash

#SBATCH --job-name=sugarScapeBatchTest
#SBATCH --ntasks=8
#SBATCH --cpus-per-task=24
#SBATCH --time=00:01:00
#SBATCH --error=./MNBatchOutput/%j-sugarScape-mpi.err
#SBATCH --output=./MNBatchOutput/%j-sugarScape-mpi.out

module purge

module load singularity/3.5.2
module load impi/2017.4           # Intel MPI implementation

mpirun singularity exec --pwd ~/PANDORA/examples/sugarScape ~/genajim_pandora.sif ./sugarScape