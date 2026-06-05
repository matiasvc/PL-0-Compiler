#!/bin/sh

echo PL/0 compiler test suite
echo ========================

fails=0

for i in *.pl0 ; do
  /usr/bin/printf "%.4s... " $i
  ../build/pl0c $i > /dev/null 2>&1
  if [ $? -eq 0 ] ; then
    echo ok
  else
    echo fail
    fails=$((fails + 1))
  fi
done

exit $fails
