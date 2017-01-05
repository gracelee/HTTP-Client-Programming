#!/bin/sh 

clear
echo "bash file begins"
echo "--------------------"
 

 for i in $(seq 1 1);
    do
         exec ./downloader test.txt $i download 
    done
