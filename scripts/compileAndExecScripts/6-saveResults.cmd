#!/bin/bash

if [ $# -lt 2 ]; then
  echo "Usage: $0 simulationBaseDir destinationDir"
  echo "* simulationBaseDir: the absolute path to the base directory in which the simulation has been run."
  echo "* destinationDir: the absolute path to the destination directory in which all the results should be copied."
  echo "  Example: ./saveResults.cmd /media/M2/gnavarro/PANDORA/ /media/HDD/gnavarro/forFer/18000s-32Ka/sim2/"
  exit 0
fi

simulationBaseDir="$1"
destinationDir="$2"

read -p "Do you want to overwrite ALL the data at $destinationDir (y/n)" yn

case $yn in
	[yY] ) echo Saving the last simulation... \(considering output.txt and config627.xml files\);
		rm -rf $destinationDir
		mkdir -p $destinationDir
		
		cp ${simulationBaseDir}/scripts/compileAndExecScripts/output.txt $destinationDir
		cp ${simulationBaseDir}/examples/IoTwins/configFiles/config627.xml $destinationDir
		cp -R ${simulationBaseDir}/examples/IoTwins/ $destinationDir
		cp -R ${simulationBaseDir}/src/ $destinationDir	

		echo "Done";;			

	[nN] ) echo Cancelled.;
			exit;;

	* ) echo Invalid response;;
esac
