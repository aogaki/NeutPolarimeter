#!/bin/bash

macFile="tmp.mac"
echo "/run/beamOn 100000000" > $macFile

for ((i=3;i<=20;i++)) do
    # ene=`echo "scale=1; $i / 10.0" | bc`
    # echo $ene

    ene=$i
    ./polarymeter -e $ene -m $macFile

    hadd -f pol$ene.root result_t*

done
