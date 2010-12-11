#!/bin/bash

rm statistics.out

for size in 10 50 100 500 1000 3000 5000 
do
	for p in 2 5 9
	do
		echo "Size $size and process $p"
		c=1
		while [ $c -le 10 ]
		do
			/usr/bin/time -f "size=$size p=$p time=%e" ./run.sh $p "test$size.in" "test$size.out" 2>> statistics.out
			((c++))
		done
	done
done
