#!/bin/bash

if [ $# -lt 2 ]; then
  echo "Usage: $0 modelName [debug]"
  echo "* modelName: the name of the model to be compiled."
  echo "* debug: debug=true if need to be compiled with the -g option."
  exit 0
fi

cd ~/PANDORA/examples/"$1"/
scons "${2:-}"
