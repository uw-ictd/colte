#!/bin/bash
FILE=../output/system-health-output.txt

rm $FILE

cat /proc/cpuinfo >> $FILE
echo ----- >> $FILE

cat /proc/meminfo >> $FILE
echo ----- >> $FILE

df >> $FILE