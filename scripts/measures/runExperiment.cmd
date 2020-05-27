#!/bin/bash

if [ $# -lt 3 ]; then
  echo "Usage: $0 timeLimit modelName inputParams"
  echo "* timeLimit: maximum time in seconds to kill the execution."
  echo "* modelName: name of the binary to be executed in the examples/<modelName>/ directory."
  echo "* inputParams: name of the XML configuration file to be used, without the extension. It should be in ./<modelName>-config/ directory."
  exit 0
fi

timeLimit=$1
modelName="$2"
inputParams="$3"

inputConfigFileName="./$modelName-config/$inputParams.xml"

mv ../examples/$modelName/config.xml ../examples/$modelName/config-backup.xml
cp $inputConfigFileName ../examples/$modelName/config.xml

cd ../examples/$modelName
./$modelName > output.txt &

systemPageSize=$(getconf PAGESIZE)
systemHertz=$(getconf CLK_TCK)
IFS=' ' # Input field separator (default is already ' ')

processPID=$!

elapsedSeconds=0

echo "$inputParams:Timestamp,$inputParams:CPU,$inputParams:AllocMem,$inputParams:AddressMem"
echo "0,0,0,0"

processDone=false
while [ "$processDone" != "true" ]; do

  sleep 1 
  
  let "elapsedSeconds+=1"
  if [ $elapsedSeconds -gt $timeLimit ]; then
    kill -SIGKILL $processPID
    break
  fi
  
  totalCPUTime=0
  uptime=0
  starttime=0

  processFile="/proc/"${processPID}"/stat"
  if test -f "$processFile"; then
    fieldIndex=1
    read -ra ADDR <<< "$(cat $processFile)"
    for i in "${ADDR[@]}"; do
      if [ $fieldIndex -eq 24 ]; then        # http://man7.org/linux/man-pages/man5/proc.5.html (section /proc/[pid]/stat
        allocMem=$(bc <<< "scale=4; $i * $systemPageSize/10^6")
        #echo "MEM (allocated):   $allocMem MB"
      elif [ $fieldIndex -eq 23 ]; then
        addressMem=$(bc <<< "scale=4; $i/10^6")
        #echo "MEM (addressable): $addressMem MB"
      elif [ $fieldIndex -eq 14 ] || [ $fieldIndex -eq 15 ] || [ $fieldIndex -eq 16 ] || [ $fieldIndex -eq 17 ]; then
        let "totalCPUTime+=$i"
      elif [ $fieldIndex -eq 22 ]; then
        let "starttime=$i"
      fi
      let "fieldIndex+=1"
    done  # for
    
    totalCPUTime=$(bc <<< "scale=2; 100 * (($totalCPUTime / $systemHertz) / $elapsedSeconds)")
    #echo "CPU (%Usage):      $totalCPUTime"
    echo "$elapsedSeconds,$totalCPUTime,$allocMem,$addressMem"
  else
    processDone=true
    echo "$elapsedSeconds,0,0,0"
  fi
done  # while

mv config-backup.xml config.xml
