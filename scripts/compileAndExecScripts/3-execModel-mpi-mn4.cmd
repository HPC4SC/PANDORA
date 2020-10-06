#!/bin/bash

if [ $# -lt 2 ]; then
  echo "Usage: $0 modelName numberOfTasks debug"
  echo "* modelName: the name of the model to be executed."
  echo "* numberOfTasks: integer indicating the number of MPI processes."
  exit 0
fi

numberOfTasks=$2

cd ~/PANDORA/examples/"$1"/
mpirun -n $numberOfTasks -bootstrap ssh ./"$1"
