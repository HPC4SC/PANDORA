#!/bin/bash

if [ $# -lt 1 ]; then
  echo "Usage: $0 debug"
  echo "* debug: debug=true to compile with option -g."
  exit 0
fi

debugMode=""
if [ $1 == "debug=true" ]; then
  debugMode="-D debug=true"
fi

cd $DEPLOYMENT_PATH/PANDORA/
rm -rf build/ install/
mkdir build/ install/

cd build/
cmake $debugMode -D CMAKE_INSTALL_PREFIX=$PANDORAPATH ../
