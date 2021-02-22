#!/bin/bash

# This script must be executed 
# from his current directory

cd ./tests/QF_UF
COUNT=0
NOT_OK=0
for directory in $(ls); do 
  if [ -d $directory ]; then
    cd $directory
    for smt_file in $(ls); do
      if [ -f $smt_file ]; then
        COUNT=$(($COUNT + 1))
        LOCAL_CMD="./../../qf_uf_test $smt_file"
        eval "$LOCAL_CMD" 
        echo "Processing file: " $smt_file
        RESULT=$?
        if [ ! "$RESULT" -eq "0" ]; then
          echo "not ok" $(pwd) $smt_file $RESULT
          NOT_OK=$(($NOT_OK + 1))
        fi
      fi
    done
    cd ../
  fi
done

echo "Number of processed cases: " $COUNT
echo "There are " $NOT_OK " problems"
