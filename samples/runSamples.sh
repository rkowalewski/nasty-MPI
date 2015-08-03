#!/bin/bash

#export LD_PRELOAD=./lib/libnasty_mpi.so;
error=0;

for file in $(find bin -type f -executable);
do
  echo "-------------Running Test: ${file} ---------------" >> samples/tests.log;
  mpirun -n 48 ${file}>>samples/tests.log 2>&1;
  if [ $? -eq 0 ];
  then
    echo $file PASS;
  else
    error=1;
    echo $file ERROR;
  fi
done

if [ ${error} -eq 1 ];
then
  echo "There happend some errors: please see samples/tests.log"
fi
