#!/bin/bash

if [ $# -lt 3 ]; then
  echo "Usage: $0 modelName xmlConfigFile debug"
  echo "* modelName: the name of the model to be executed."
  echo "* xmlConfigFile: the name of the XML configuration file to be used."
  echo "* gdbgui: gdbgui=true if gdbgui need to be launched."
  echo "EXAMPLE: ./3-execModel.cmd IoTwins configFiles/config.xml gdbgui=true"
  exit 0
fi

gdbguiMode=""
if [ $3 == "gdbgui=true" ]; then
  gdbguiMode="gdbgui -r"
fi

cd ~/PANDORA/examples/"$1"/
$gdbguiMode ./"$1"
