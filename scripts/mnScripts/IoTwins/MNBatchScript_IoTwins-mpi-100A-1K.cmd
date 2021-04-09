#!/bin/bash

#SBATCH --job-name=IoTwinsTest
#SBATCH --qos=debug
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --time=02:00:00
#SBATCH --error=./../MNBatchOutput/%j-IoTwins-mpi.err
#SBATCH --output=./../MNBatchOutput/%j-IoTwins-mpi.out

module purge

module load singularity/3.5.2
module load impi/2017.4           # Intel MPI implementation

modelName="IoTwins"
experimentID="$SLURM_JOBID"
numberOfAgents="13090"
numberOfTasks="$SLURM_NTASKS"
configFileName="config-${numberOfAgents}A-${numberOfTasks}K.xml"
echo $modelName
echo $configFileName
echo $experimentID
python updateXMLConfigFileOutputPaths.py $modelName $configFileName $experimentID

singularity exec -B /gpfs/home/bsc21/bsc21473/PANDORA/install/lib:/.singularity.d/libs --pwd ~/PANDORA/scripts/compileAndExecScripts/ ~/genajim_pandora.sif ./3-execModel-mpi-mn4.cmd $modelName $configFileName $numberOfTasks
