#!/bin/bash
FILE=../output/system-health-output.txt

rm $FILE

systemctl status colte-hss >> $FILE
echo ----- >> $FILE

systemctl status colte-mme >> $FILE
echo ----- >> $FILE

systemctl status colte-spgw >> $FILE
echo ----- >> $FILE

systemctl status haulage >> $FILE
echo ----- >> $FILE

systemctl status colte-webgui >> $FILE
echo ----- >> $FILE

cat /proc/cpuinfo >> $FILE
echo ----- >> $FILE

cat /proc/meminfo >> $FILE
echo ----- >> $FILE

df >> $FILE

