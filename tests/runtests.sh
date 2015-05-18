echo "Running unit tests:"

#find all *_tests but exclude mac os debugging symbol
for i in $(find . -type f \( -name \*_tests ! -path .\/\*dSYM\/\* \));
do
  if test -f $i
  then
    if ./$i 2>>tests/tests.log
    then
      echo $i PASS
    else
      echo "ERROR in test $i: here's tests/tests.log"
      echo "------"
      tail tests/tests.log
      exit 1
    fi
  fi
done

echo ""
