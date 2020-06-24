#!/bin/bash

if [ $# -lt 3 ]; then
  echo "Usage: $0 modelName numberOfTasks debug"
  echo "* modelName: the name of the model to be executed."
  echo "* numberOfTasks: integer indicating the number of MPI processes."
  echo "* debug: debug=true if gdbgui need to be launched."
  exit 0
fi

numberOfTasks=$2

gdbguiMode=""
if [ $3 == "debug=true" ]; then
  gdbguiMode="gdbgui -r"
fi

cd ~/PANDORA/examples/"$1"/
mpirun -n $numberOfTasks $gdbguiMode ./"$1"
