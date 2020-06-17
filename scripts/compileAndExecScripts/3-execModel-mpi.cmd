#!/bin/bash

if [ $# -lt 3 ]; then
  echo "Usage: $0 modelName numberOfTasks debug"
  echo "* modelName: the name of the model to be executed."
  echo "* numberOfTasks: integer indicating the number of MPI processes."
  echo "* debug: debug=true if gdbgui need to be launched."
  exit 0
fi

numberOfTasks=${2:-1}

gdbguiMode=""
if [ $2 == "debug=true" ]; then
  gdbguiMode="gdbgui -r"
fi

cd ~/PANDORA/examples/"$1"/
$gdbguiMode mpirun -n $numberOfTasks ./"$1"
