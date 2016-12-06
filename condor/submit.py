#!/bin/python
import glob, os, sys,subprocess,cPickle
import commands
import time
import random


#__________________________________________________________
def getCommandOutput(command):
    p = subprocess.Popen(command, shell = True, stdout = subprocess.PIPE, stderr = subprocess.PIPE)
    stdout,stderr = p.communicate()
    return {"stdout":stdout, "stderr":stderr, "returncode":p.returncode}


#__________________________________________________________
def SubmitToBatch(cmd,nbtrials):
    submissionStatus=0
    for i in xrange(nbtrials):            
        outputCMD = getCommandOutput(cmd)
        stderr=outputCMD["stderr"].split('\n')
        stdout=outputCMD["stdout"].split('\n')

        if len(stderr)==1 and stderr[0]=='' :
            print "------------GOOD SUB"
            submissionStatus=1
        else:
            print "++++++++++++ERROR submitting, will retry"
            print "Trial : "+str(i)+" / "+str(nbtrials)
            print "stderr : ",stderr
            print "stderr : ",len(stderr)

            time.sleep(10)

            
        if submissionStatus==1:
            return 1
        
        if i==nbtrials-1:
            print "failed sumbmitting after: "+str(nbtrials)+" trials, will exit"
            return 0


#__________________________________________________________
if __name__=="__main__":
    nbjobsSub=0
    NJOBS=10
    EVTMAX=500
    BFIELD=1
  
    for i in xrange(NJOBS):
        print i
        os.system('sed -i \"2s/.*/arguments             = %i %i %i \$\(ClusterID\) \$\(ProcId\)/g\"  submit_minbias.sub'%(EVTMAX, BFIELD, i))
        print 'sed -i \"2s/.*/arguments             = %i %i %i \$\(ClusterID\) \$\(ProcId\)/g\"  submit_minbias.sub'%(EVTMAX, BFIELD, i)
        cmdBatch="condor_submit submit.sub"
        job=SubmitToBatch(cmdBatch,10)
        nbjobsSub+=job

   
    print 'succesfully sent %i  jobs'%nbjobsSub
