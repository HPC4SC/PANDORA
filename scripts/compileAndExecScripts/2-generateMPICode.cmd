#!/bin/bash

if [ $# -lt 1 ]; then
  echo "Usage: $0 modelName [debug]"
  echo "* modelName: the name of the model to be compiled."
  exit 0
fi

cd ~/PANDORA/examples/"$1"/
python GenerateMPICodeAndCompileModel.py
