#!/bin/bash

#export LD_PRELOAD=./lib/libnasty_mpi.so;
rm -f samples/tests.log
error=0
tests=()

for file in $(find bin -type f -executable);
do
  tests=("${tests[@]}" "$file");
done

total=${#tests[@]}

for ((i=0; i < $total; i++))
do
  file=${tests[i]};
  echo "-------------Running Test: ${file} ---------------" >> samples/tests.log;
  #echo "-------------Running Test: ${file} ---------------";
  counter=0;
  while [ ${error} -eq 0 -a ${counter} -lt 10 ];
  do
    mpirun -n 48 ${file}>>samples/tests.log 2>&1;
    if [ $? -ne 0 ];
    then
      error=1;
      echo $file ERROR;
    fi
    counter=$((counter + 1));
  done

  if [ ${error} -eq 0 ]
  then
    echo $file PASS;
  fi

  if [ $i -ne $((total - 1)) ]
  then
    error=0;
  fi
done

if [ ${error} -eq 1 ];
then
  echo "There happend some errors: please see samples/tests.log"
fi
