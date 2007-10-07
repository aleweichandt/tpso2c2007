#!/bin/sh
lineas=`ps -A|grep adp|wc -l|cut -d" " -f7`
echo $lineas
if [ $lineas != "0" ]; then
echo "ADP ya esta corriendo!"
else
./adp
fi

