#!/bin/bash
0;95;0c
{ echo "START"; \
  sleep 5; \
  echo "STOP"; \
  sleep 2; \
  echo "OFF";} \
     | sudo ./lab4b --log=test_log.txt

if [ $? -ne 0 ]
then
    echo "Error: correct exit code is 0"
else
    echo "Correct exit value!"
fi

echo ""
echo "Contents of log for visual check"
cat test_log.txt
echo""

for c in START STOP OFF SHUTDOWN
do
    grep "$c" test_log.txt > /dev/null
    if [ $? -ne 0 ]
    then
	echo "Problem logging $c"
    else
	echo "$c was logged correctly"
    fi
done

rm -f test_log.txt
