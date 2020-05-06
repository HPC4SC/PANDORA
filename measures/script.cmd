#!/bin/bash

cd ../examples/sugarScape/
./sugarScape > output.txt &

processPID=$!

systemPageSize=$(getconf PAGESIZE)
systemHertz=$(getconf CLK_TCK)
IFS=' '

elapsedSeconds=0

echo "0,0,0"

processDone=false
while [ "$processDone" != "true" ]; do

  sleep 1 
  let "elapsedSeconds+=1"
  if [ $elapsedSeconds = 20 ]; then
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
      if [ $fieldIndex = 24 ]; then        # http://man7.org/linux/man-pages/man5/proc.5.html (section /proc/[pid]/stat
        allocMem=$(bc <<< "scale=4; $i * $systemPageSize/10^6")
        #echo "MEM (allocated):   $allocMem MB"
      elif [ $fieldIndex = 23 ]; then
        addressMem=$(bc <<< "scale=4; $i/10^6")
        #echo "MEM (addressable): $addressMem MB"
      elif [ $fieldIndex = 14 ] || [ $fieldIndex = 15 ] || [ $fieldIndex = 16 ] || [ $fieldIndex = 17 ]; then
        let "totalCPUTime+=$i"
      elif [ $fieldIndex = 22 ]; then
        let "starttime=$i"
      fi
      let "fieldIndex+=1"
    done  # for
    
    totalCPUTime=$(bc <<< "scale=2; 100 * (($totalCPUTime / $systemHertz) / $elapsedSeconds)")
    #echo "CPU (%Usage):      $totalCPUTime"
    echo "$totalCPUTime,$allocMem,$addressMem"
  else
    processDone=true
  fi
done  # while

rm ./output.txt
kill -SIGKILL $processPID
