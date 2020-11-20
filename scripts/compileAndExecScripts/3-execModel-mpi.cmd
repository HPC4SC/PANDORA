#!/bin/bash

if [ $# -lt 3 ]; then
  echo "Usage: $0 modelName numberOfTasks debug"
  echo "* modelName: the name of the model to be executed."
  echo "* numberOfTasks: integer indicating the number of MPI processes."
  echo "* configFileName: string indicating the name of the config file wanted to use."
  exit 0
fi

numberOfTasks=$2

cd ~/PANDORA/examples/"$1"/
mpirun -n $numberOfTasks ./"$1" $2 $3
