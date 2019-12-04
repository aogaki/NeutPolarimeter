#!/bin/bash

# Get data from RKIN Code for 2-Body Relativistic Kinematics Calculations

for ((i=23;i<=200;i++)) do
    ene=`echo "scale=1; $i / 10.0" | bc`
    echo $ene

    curl -X POST http://www.tunl.duke.edu/rkin.php -d "anginit=0&angfinal=180&angstep=1&energy=$ene&reaction=2H(g%2Cn)1H&excit=0&Submit=Run+RKIN+code" > $ene.dat
done


