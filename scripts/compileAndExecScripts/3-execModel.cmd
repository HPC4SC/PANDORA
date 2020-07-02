#!/bin/bash

if [ $# -lt 2 ]; then
  echo "Usage: $0 modelName debug"
  echo "* modelName: the name of the model to be executed."
  echo "* gdbgui: gdbgui=true if gdbgui need to be launched."
  exit 0
fi

gdbguiMode=""
if [ $2 == "gdbgui=true" ]; then
  gdbguiMode="gdbgui -r"
fi

cd ~/PANDORA/examples/"$1"/
$gdbguiMode ./"$1"