#!/bin/bash

if [ $# -lt 1 ]; then
  echo "Usage: $0 modelName"
  echo "* modelName: the name of the model to be cleaned."
  exit 0
fi

cd ~/PANDORA/examples/"$1"/
scons -c
