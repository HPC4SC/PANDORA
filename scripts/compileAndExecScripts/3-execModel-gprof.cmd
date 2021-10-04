#!/bin/bash

if [ $# -lt 2 ]; then
  echo "Usage: $0 modelName xmlConfigFile numberOfTasks"
  echo "* modelName: the name of the model to be executed."
  echo "* xmlConfigFile: the name of the XML configuration file to be used (at ./configFiles/)."
  #echo "* numberOfTasks: integer indicating the number of MPI processes."
  #echo "EXAMPLE: ./3-execModel-mpi.cmd IoTwins config.xml 4"
  echo "EXAMPLE: ./3-execModel-mpi.cmd IoTwins config.xml"
  exit 0
fi

#numberOfTasks=$3

cd $DEPLOYMENT_PATH/PANDORA/examples/"$1"/
./"$1" ./configFiles/"$2"

cd $DEPLOYMENT_PATH/PANDORA/scripts/compileAndExecScripts/
gprof $DEPLOYMENT_PATH/PANDORA/examples/"$1"/"$1" $DEPLOYMENT_PATH/PANDORA/examples/"$1"/gmon.out > profile-data.txt
