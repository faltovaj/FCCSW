#!/bin/sh -u
# set FCCSWBASEDIR to the directory containing this script
export FCCSWBASEDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
weekday=`date +%a`
source /cvmfs/fcc.cern.ch/sw/views/releases/externals/93.0.0/x86_64-slc6-gcc62-opt/setup.sh
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/cvmfs/sft.cern.ch/lcg/releases/DD4hep/01-05-2396f/x86_64-slc6-gcc62-opt/lib/
add_to_path LD_LIBRARY_PATH /afs/cern.ch/user/j/jcervant/public/xerces/lib/