#!/bin/bash

varTemp=`date +%s`
#echo $var
filePath='/home/whu-vmfunc/lbrinfo/'
var=$filePath$varTemp
xentrace -D -e 0x00082031 -T 1 $var.dat
cat $var.dat | xentrace_format /home/whu-vmfunc/lbrinfo/formats.spec4lbr > $var.txt
python /home/whu-vmfunc/lbrinfo/transform-view1.py $varTemp
cat $var.json 
#cat $var.json | xargs echo -e
rm $var.dat
rm $var.txt
