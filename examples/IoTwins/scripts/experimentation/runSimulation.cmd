#!/bin/bash

if [ $# -lt 2 ]; then
  echo "Usage: $0 timeLimit modelName inputParams"
  echo "* modelName: name of the binary to be executed in the examples/<modelName>/ directory."
  echo "* inputParams: name of the XML configuration file to be used, without the extension. It should be in ./<modelName>-config/ directory."
  exit 0
fi

modelName="$1"
inputParams="$2"
outputRun="run_$2"

inputConfigFileName="./$modelName-config/$inputParams.xml"

mv ~/PANDORA/examples/$modelName/config.xml ~/PANDORA/examples/$modelName/config-backup.xml
cp $inputConfigFileName ~/PANDORA/examples/$modelName/config.xml

cd ~/PANDORA/examples/$modelName
./$modelName > ./scripts/experimentation/runLogs/$outputRun.txt