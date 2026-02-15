read -p "How many prime numbers ?: " num

c=0
k=0
n=2
numero=$[$num-1]
while [ $k -ne $num ]; do
     for i in `seq 1 $n`;do
        r=$[$n%$i]
                if [ $r -eq 0 ]; then
                        c=$[$c+1]
                fi
     done

      if [ $c -eq 2 ]; then
              echo "$i"
              k=$[$k+1]
        fi
        n=$[$n+1]
        c=0
done
