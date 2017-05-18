#!/bin/bash

export jobOption=Reconstruction/RecCalorimeter/tests/options/combinedReconstruction_SW_pileup.py

export ENERGY=100
export BFIELD=1

for i in `seq 0 19`; do
    sed -i "2s/.*/energy=$ENERGY/g" ${jobOption}
    sed -i "3s/.*/parti=$i/g" ${jobOption}
    sed -i "4s/.*/bfield=$BFIELD/g" ${jobOption}
    cat ${jobOption}
    ./run gaudirun.py ${jobOption};
done
