#!/bin/bash

if [ $# -lt 1 ]; then
  echo "Usage: $0 modelName"
  echo "* modelName: name of the binary to be executed in the examples/<modelName>/ directory."
  exit 0
fi

modelName="$1"

cd ./configFiles/$modelName/
filesList=*

for inputParamsFileName in $filesList
do
	inputParams=$(echo $inputParamsFileName | cut -d'.' -f 1)
	echo $inputParams

	cd ~/PANDORA/scripts/measures/PythonGraphs/

	mv ~/PANDORA/examples/$modelName/config.xml ~/PANDORA/examples/$modelName/config-backup.xml
	cp ./configFiles/$modelName/$inputParamsFileName ~/PANDORA/examples/$modelName/config.xml

	for numberOfProcesses in 1 2 4
	do
		echo "Executing $modelName with params $inputParams with $numberOfProcesses MPI process(es):"
		
		cd ~/PANDORA/scripts/compileAndExecScripts/
		./3-execModel-mpi.cmd $modelName $numberOfProcesses

		cd ~/PANDORA/scripts/measures/PythonGraphs
		python3 makePerformanceGraphs.py $modelName $inputParams $numberOfProcesses
	done

	cd ~/PANDORA/examples/$modelName/
	mv config-backup.xml config.xml
done