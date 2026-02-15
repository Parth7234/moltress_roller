echo "Enter a string to be entered:"
read str
echo
len=`echo $str | wc -c`
len=`expr $len - 1`
i=1
j=`expr $len / 2`

while test $i -le $j
do
echo "the value of i is $i"
done

