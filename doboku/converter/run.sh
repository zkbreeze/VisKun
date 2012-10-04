#! /bin/sh

s=1

while [ $s -ne 10 ]
do
./converter ../mixbox16/u3db0000${s}d3.d ../mixbox16/x3d.d pressure0${s}.kvsml velocity0${s}.kvsml
s=`expr $s + 1`
done

s=10

while [ $s -ne 21 ]
do
./converter ../mixbox16/u3db000${s}d3.d ../mixbox16/x3d.d pressure${s}.kvsml velocity${s}.kvsml
s=`expr $s + 1`
done
