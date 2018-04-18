#!/bin/sh -u
# set FCCSWBASEDIR to the directory containing this script
export FCCSWBASEDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
weekday=`date +%a`
source /cvmfs/fcc.cern.ch/testing/sw/views/stable/x86_64-slc6-gcc62-opt/setup.sh
# library path for py converter
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/afs/cern.ch/work/c/cneubuse/public/FCC_calo_analysis_cpp/install/lib/
add_to_path LD_LIBRARY_PATH /afs/cern.ch/user/j/jcervant/public/xerces/lib/
