#!/bin/bash
echo "enter a value"
read count
n=100
if [ $count-eq$n ]
then
echo "Count is 100"
elif [ $count -gt $n ]
then
echo "Count is greater than 100"
else
echo "Count is less than 100"
fi
