#!/bin/bash

#SBATCH --job-name=sugarScapeBatchTest
#SBATCH --ntasks=1
#SBATCH --time=00:01:00
#SBATCH --error=./MNBatchOutput/%j-sugarScape.err
#SBATCH --output=./MNBatchOutput/%j-sugarScape.out

module purge
module load singularity/3.5.2

singularity exec --pwd ~/PANDORA/examples/sugarScape ~/pandora_ext_singularity.sif ./sugarScape
