#!/bin/bash

export jobOption=condor/geant_fullsim_ecal_minbias.py
export suffix=tracker_cryo_v0

export EVTMAX=$1
export BFIELD=$2
export i=$3

#path of the software
echo "Export paths"
export SOFTWARE_PATH_AFS=/afs/cern.ch/user/h/helsens/FCCsoft/FCCSOFT/newEDM2/FCCSW_Jana/
export CMTCONFIG=x86_64-slc6-gcc49-opt

echo "Copy a working directory"
cp -r $SOFTWARE_PATH_AFS .

cd FCCSW_Jana/

echo "Init"
#source the script
source init.sh afs

#add job options to the python script
sed -i "8s/.*/num_events=$EVTMAX/g" ${jobOption}
sed -i "9s/.*/magnetic_field=$BFIELD/g" ${jobOption}
sed -i "10s/.*/i=$i/g" ${jobOption}

echo "Cat "${jobOption}
cat ${jobOption}

echo "run"
LD_PRELOAD=build.$BINARY_TAG/lib/libDetSegmentation.so ./run gaudirun.py ${jobOption}

echo "LS:"
ls -l

echo "Copy output file to eos"
export EOS_MGM_URL="root://eospublic.cern.ch"
source /afs/cern.ch/project/eos/installation/client/etc/setup.sh
xrdcp output.root root://eospublic//eos/fcc/users/h/helsens/newgeometry_gev_15nov/output_minbias_bfield${BFIELD}_part${i}_Lar4mm_Pb2mm_${suffix}.root

echo "Cleaning"
cd ../
rm -rf FCCSW_Jana
