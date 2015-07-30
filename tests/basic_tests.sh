echo "Running unit tests:"

for i in $(find . -type f \( -name \*_tests ! -path .\/\*dSYM\/\* \));
do
  if $VALGRIND $i 2>> tests/tests.log;
  then
    echo $i PASS
  else
    echo "ERROR in test $i: here's tests/tests.log"
    echo "------"
    tail tests/tests.log
    exit 1
  fi
done

echo ""
