#!/bin/bash
set -x #echo on
for ((k=1; k<=4; k++)); do
	for filename in ./Data/*.sop; do
		for ((t=1; t<=4; t++)); do
			for ((i=5000; i<=20000; i+=5000)); do
				./optimizer.exe "1" "$t" "$i" "$k" "$filename"
			done
		done
	done
done

for ((k2=4; k2<=8; k2++)); do
	for filename2 in ./Data/*.sop; do
		for ((t2=1; t2<=4; t2++)); do
			for ((i2=5000; i2<=20000; i2+=5000)); do
				./optimizer.exe "2" "$t2" "$i2" "$k2" "$filename2"
			done
		done
	done
done
