#!/bin/bash

#iteration number - begins with 1 (also used for the seed)
export NJOBS=20
#total number of jobs to submit
export EVTMAX=500
export BFIELD=1
export ETAMIN=-1.5
export ETAMAX=1.5
#0 off, 1 on

for ENE in 10 20 50 100
do
  for i in `seq 1 $NJOBS`
  do
      echo $i
      if [ ${ENE} -le 200 ]
      then
	  sed -i "2s/.*/arguments             = $((${ENE}*1000)) ${ETAMIN} ${ETAMAX} ${EVTMAX} ${BFIELD} ${i} \$\(ClusterID\) \$\(ProcId\)/g"  submit.sub 
	  condor_submit submit.sub
      else
	  sed -i "2s/.*/arguments             = $((${ENE}*1000)) ${ETAMIN} ${ETAMAX} ${EVTMAX} ${BFIELD} ${i} \$\(ClusterID\) \$\(ProcId\)/g"  submit_long.sub 
	  condor_submit submit_long.sub
      fi
  done
done
