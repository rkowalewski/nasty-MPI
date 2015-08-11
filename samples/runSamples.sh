#!/bin/bash

#export LD_PRELOAD=./lib/libnasty_mpi.so;
rm -f samples/tests.log

tests=()
submit_times=('maximum_delay' 'fire_immediate')
submit_orders=('random_order' 'program_order' 'get_after_put' 'put_after_get')


if [ -z "${NPROCS}" -o -z "${NPROCS_PER_NODE}" ];
then
  echo "usage: $0 <nprocs> <nprocs_per_node>"
  exit 1
fi

function run_tests()
{
  counter=0;
  error=0;
  while [ ${counter} -lt 5 -a ${error} -eq 0 ];
  do
    mpirun -n ${NPROCS} ${1} ${NPROCS_PER_NODE} >> samples/tests.log 2>&1;
    error=$?
    counter=$((counter + 1));
  done
  return ${error};
}

for file in $(find bin -type f -executable);
do
  tests=("${tests[@]}" "$file");
done

total=${#tests[@]}
error_global=0

for ((i=0; i < $total; i++))
do
  file=${tests[i]};
  echo "-------------RUNNING TEST (File: ${file})---------------" | tee -a samples/tests.log;
  error_file=0;
  if [ -z "${LD_PRELOAD}" ];
  then
    run_tests ${file};
    if [ $? -ne 0 ];
    then
      error_file=1;
    fi
  else
    for submit_time in ${submit_times[@]};
    do
      export SUBMIT_TIME="${submit_time}";
      echo "submit_time: ${submit_time}" | tee -a samples/tests.log;
      if [ ${submit_time} == "fire_immediate" ];
      then
        run_tests ${file};
        if [ $? -ne 0 ];
        then
          echo "submit_time: ${submit_time} --> ERROR" | tee -a samples/tests.log;
          error_file=1;
        else
          echo "submit_time: ${submit_time} --> PASS";
        fi
      else
        for submit_order in ${submit_orders[@]};
        do
          export SUBMIT_ORDER="${submit_order}";
          run_tests ${file};
          if [ $? -ne 0 ];
          then
            echo "--submit_order: ${submit_order} --> ERROR" | tee -a samples/tests.log;
            error_file=1;
          else
            echo "--submit_order: ${submit_order} --> PASS";
          fi
        done
      fi
    done
  fi  

  if [ ${error_file} -ne 0 ]
  then
    error_global=1;
  fi

  error_file=0;
  echo '-------------TEST ENDED---------------' | tee -a samples/tests.log;
done

if [ ${error_global} -eq 1 ];
then
  echo "There happend some errors: please see samples/tests.log"
fi
