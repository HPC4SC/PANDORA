#!/bin/bash

#SBATCH --job-name=sugarScapeBatchTest
#SBATCH --ntasks=1
#SBATCH --time=00:01:00
#SBATCH --error=./MNBatchOutput/%j-sugarScape_escalable.err
#SBATCH --output=./MNBatchOutput/%j-sugarScape_escalable.out

module purge
module load singularity/3.5.2

singularity exec --pwd ~/PANDORA/scripts/measures ~/genajim_pandora.sif ./mainScript.cmd
