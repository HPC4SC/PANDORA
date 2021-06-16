#!/bin/bash

if [ ! -d "./../MNBatchOutput/" ]; then
  mkdir ./../MNBatchOutput/
fi

sbatch MNBatchScript_IoTwins-mpi-1K.cmd 100 500
sbatch MNBatchScript_IoTwins-mpi-1K.cmd 500 500
sbatch MNBatchScript_IoTwins-mpi-1K.cmd 1000 500
sbatch MNBatchScript_IoTwins-mpi-1K.cmd 2000 500
sbatch MNBatchScript_IoTwins-mpi-1K.cmd 4000 500
sbatch MNBatchScript_IoTwins-mpi-1K.cmd 8000 500
sbatch MNBatchScript_IoTwins-mpi-1K.cmd 16000 500
sbatch MNBatchScript_IoTwins-mpi-1K.cmd 32000 500
sbatch MNBatchScript_IoTwins-mpi-1K.cmd 50000 500

sbatch MNBatchScript_IoTwins-mpi-2K.cmd 100 500
sbatch MNBatchScript_IoTwins-mpi-2K.cmd 500 500
sbatch MNBatchScript_IoTwins-mpi-2K.cmd 1000 500
sbatch MNBatchScript_IoTwins-mpi-2K.cmd 2000 500
sbatch MNBatchScript_IoTwins-mpi-2K.cmd 4000 500
sbatch MNBatchScript_IoTwins-mpi-2K.cmd 8000 500
sbatch MNBatchScript_IoTwins-mpi-2K.cmd 16000 500
sbatch MNBatchScript_IoTwins-mpi-2K.cmd 32000 500
sbatch MNBatchScript_IoTwins-mpi-2K.cmd 50000 500

sbatch MNBatchScript_IoTwins-mpi-4K.cmd 100 500
sbatch MNBatchScript_IoTwins-mpi-4K.cmd 500 500
sbatch MNBatchScript_IoTwins-mpi-4K.cmd 1000 500
sbatch MNBatchScript_IoTwins-mpi-4K.cmd 2000 500
sbatch MNBatchScript_IoTwins-mpi-4K.cmd 4000 500
sbatch MNBatchScript_IoTwins-mpi-4K.cmd 8000 500
sbatch MNBatchScript_IoTwins-mpi-4K.cmd 16000 500
sbatch MNBatchScript_IoTwins-mpi-4K.cmd 32000 500
sbatch MNBatchScript_IoTwins-mpi-4K.cmd 50000 500

#sbatch MNBatchScript_IoTwins-mpi-8K.cmd 100 540
#sbatch MNBatchScript_IoTwins-mpi-8K.cmd 500 540
#sbatch MNBatchScript_IoTwins-mpi-8K.cmd 1000 540
#sbatch MNBatchScript_IoTwins-mpi-8K.cmd 2000 540
#sbatch MNBatchScript_IoTwins-mpi-8K.cmd 4000 540
#sbatch MNBatchScript_IoTwins-mpi-8K.cmd 8000 540
#sbatch MNBatchScript_IoTwins-mpi-8K.cmd 16000 540
#sbatch MNBatchScript_IoTwins-mpi-8K.cmd 32000 540
#sbatch MNBatchScript_IoTwins-mpi-8K.cmd 50000 540
#
#sbatch MNBatchScript_IoTwins-mpi-16K.cmd 100 540
#sbatch MNBatchScript_IoTwins-mpi-16K.cmd 500 540
#sbatch MNBatchScript_IoTwins-mpi-16K.cmd 1000 540
#sbatch MNBatchScript_IoTwins-mpi-16K.cmd 2000 540
#sbatch MNBatchScript_IoTwins-mpi-16K.cmd 4000 540
#sbatch MNBatchScript_IoTwins-mpi-16K.cmd 8000 540
#sbatch MNBatchScript_IoTwins-mpi-16K.cmd 16000 540
#sbatch MNBatchScript_IoTwins-mpi-16K.cmd 32000 540
#sbatch MNBatchScript_IoTwins-mpi-16K.cmd 50000 540
