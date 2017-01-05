#!/bin/bash

for ((j = 1; j <= 32; j += 1))
do

	for ((i = 0; i < 5; i += 1))
	do
		( /usr/bin/time -f "%e" ./downloader large.txt $j /local/dil15 ) >> data_large$j.txt 
	done
	
done

