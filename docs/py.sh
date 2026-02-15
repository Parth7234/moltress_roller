echo "Enter number of rows:"
read n

for ((i=1; i<=n; i++))
do
    # print spaces
    for ((j=1; j<=n-i; j++))
    do
        echo -ne " "
    done

    # print stars
    for ((k=1; k<=2*i-1; k++))
    do
        echo -ne "*"
    done

    echo
done

