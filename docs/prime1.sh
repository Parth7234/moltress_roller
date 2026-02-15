#!/bin/sh
read -p "check prime numbers ?: " num
c=0
k=0
for ((i=2;i<=num/2;i++)); do
	c=$[$num%$i]
         k=$[$k+$c]
	
done
if [ $k -eq 0 ]
then
	echo "number $num is not prime"
else
	echo "number $num is prime"
fi
        

