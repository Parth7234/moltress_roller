echo "enter a number:"
read x
echo $x
while(true)
do
	if ["$x" -eq "5"]; then
		echo "the number is $x"
	else 
		echo "the number is other number"
	fi
done
