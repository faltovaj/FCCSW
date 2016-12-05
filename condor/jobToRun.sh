#!/bin/bash

export jobOption=condor/geant_fullsim_ecal_singleparticles.py
export suffix=tracker_cryo_v0

export ENE=$1
export ETAMIN=$2
export ETAMAX=$3
export EVTMAX=$4
export BFIELD=$5
export i=$6

#path of the software
echo "Export paths"
export SOFTWARE_PATH_AFS=/afs/cern.ch/work/n/novaj/HEP-FCC/FCCSW
export CMTCONFIG=x86_64-slc6-gcc49-opt

echo "Copy a working directory"
cp -r $SOFTWARE_PATH_AFS .

cd FCCSW/

echo "Init"
#source the script
source init.sh afs

#add job options to the python script
sed -i "8s/.*/energy=$ENE/g"  ${jobOption}
sed -i "9s/.*/etamin=$ETAMIN/g" ${jobOption}
sed -i "10s/.*/etamax=$ETAMAX/g" ${jobOption}
sed -i "11s/.*/num_events=$EVTMAX/g" ${jobOption}
sed -i "12s/.*/magnetic_field=$BFIELD/g" ${jobOption}
sed -i "13s/.*/i=$i/g" ${jobOption}

echo "Cat "${jobOption}
cat ${jobOption}

echo "run"
LD_PRELOAD=build.$BINARY_TAG/lib/libDetSegmentation.so ./run gaudirun.py ${jobOption}

echo "LS:"
ls -l

echo "Copy output file to eos"
export EOS_MGM_URL="root://eospublic.cern.ch"
source /afs/cern.ch/project/eos/installation/client/etc/setup.sh
xrdcp output.root root://eospublic//eos/fcc/users/n/novaj/newgeometry_gev_15nov/output_e$((${ENE}/1000))GeV_etaMin${ETAMIN}_etaMax${ETAMAX}_bfield${BFIELD}_part${i}_Lar4mm_Pb2mm_${suffix}.root

echo "Cleaning"
cd ../
rm -rf FCCSW
