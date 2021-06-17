#!/bin/bash

#SBATCH --job-name=IoTwinsTest
#SBATCH --qos=bsc_case
#SBATCH --nodes=1
#SBATCH --ntasks=8
#SBATCH --cpus-per-task=1
#SBATCH --time=48:00:00
#SBATCH --error=./../MNBatchOutput/%j-IoTwins-mpi.err
#SBATCH --output=./../MNBatchOutput/%j-IoTwins-mpi.out

if [ $# -lt 1 ]; then
  echo "Usage: MNBatchScript_IoTwins-mpi-{X}K.cmd numberOfAgents"
  echo "* numberOfAgents: the number of agents to simulate."
  exit 0
fi

numberOfAgents="$1"
numberOfSteps="$2"

modelName="IoTwins"
experimentID="$SLURM_JOBID"
numberOfTasks="$SLURM_NTASKS"
configFileName="config-${numberOfAgents}A-${numberOfSteps}S-${numberOfTasks}K.xml"
python updateXMLConfigFileOutputPaths.py $modelName $configFileName $experimentID


module purge

module load singularity/3.5.2
module load impi/2017.4           # Intel MPI implementation

singularity exec -B $DEPLOYMENT_PATH/PANDORA/install/lib:/.singularity.d/libs --pwd $DEPLOYMENT_PATH/PANDORA/scripts/compileAndExecScripts/ $DEPLOYMENT_PATH/genajim_pandora.sif ./3-execModel-mpi-mn4.cmd $modelName $configFileName $numberOfTasks

echo "Simulation finished # ${experimentID}."
