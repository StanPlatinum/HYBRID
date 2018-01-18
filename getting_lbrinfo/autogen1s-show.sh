#!/bin/bash

varTemp=`date +%s`
#echo $var
filePath='/home/whu-vmfunc/lbrinfo/'
var=$filePath$varTemp
xentrace -D -e 0x00082031 -T 1 $var.dat
cat $var.dat | xentrace_format formats.spec4lbr
#python /home/whu-vmfunc/lbrinfo/transform.py $var
#cat $var.json | xargs echo -e
rm $var.dat
#rm $var.txt
