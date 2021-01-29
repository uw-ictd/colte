#!/bin/bash

BULK="$1"

while read -a line; do
	n=${#line[*]}
	echo "$n"
	if [ $n -eq 6 ]
	then
		coltedb add ${line[0]} ${line[1]} ${line[2]} ${line[3]} ${line[4]} ${line[5]}
	else
		coltedb add ${line[0]} ${line[1]} ${line[2]} ${line[3]} ${line[4]}
	fi
done < "$BULK"
