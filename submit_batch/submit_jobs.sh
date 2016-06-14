#!/bin/bash

export NEVT=500

export BFIELD=1

export ENE=100


#option -n 8 -R "span[hosts=1]" for running in parallel
for NEWDIM in 2600 1600 600 0
do
if [ ${ENE} -le 500 ]
then
    bsub -q 1nw prepare_batch.sh geant_fullsim_ecal_SPG_batch.py ${NEVT} ${ENE} ${BFIELD} ${NEWDIM}
else 
    bsub -q 1nw -n 8 -R "span[hosts=1]" prepare_batch.sh geant_fullsim_ecal_SPG_batch.py ${NEVT} ${ENE} ${BFIELD} ${NEWDIM}
fi
done
