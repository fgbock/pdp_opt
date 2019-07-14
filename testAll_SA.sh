#!/bin/bash
set -x #echo on
for filename in ./Data/*.sop; do
	for ((t=1; t<=4; t++)); do
		for ((i=5000; i<=20000; i+=5000)); do
			./optimizer.exe "2" "$t" "$i" "$filename"
		done
    done
done
