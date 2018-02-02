import os
import numpy as np

#loads array of random seeds from file
filename_array = np.loadtxt('condor/list_novaj_minBias_sort.txt',dtype='string',delimiter='\n')

#set these in the .sh script
i=0
print "File number:", i

#filename='output_condor_novaj_201801081606412074.root'

from Gaudi.Configuration import *

from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

# v01 production - min. bias events
#podioevent = FCCDataSvc( "EventDataSvc", input = '/eos/experiment/fcc/hh/simulation/samples/v01/physics/MinBias/bFieldOn/etaFull/simu/'+filename )
podioevent = FCCDataSvc( "EventDataSvc", input = '/eos/experiment/fcc/hh/simulation/samples/v01/physics/MinBias/bFieldOn/etaFull/simu/'+str(filename_array[i]))

# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import PodioInput

podioinput = PodioInput("in", collections = ["ECalBarrelCells",
                                             "ECalEndcapCells",
                                             "ECalFwdCells",
                                             "HCalBarrelCells",
                                             "HCalExtBarrelCells",
                                             "HCalEndcapCells",
                                             "HCalFwdCells",
                                             "TailCatcherCells"])

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors = [ 'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                  'file:Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml',
                  'file:Detector/DetFCChhECalInclined/compact/FCChh_ECalBarrel_withCryostat.xml',
                  'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalBarrel_TileCal.xml',
                  'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalExtendedBarrel_TileCal.xml',
                  'file:Detector/DetFCChhCalDiscs/compact/Endcaps_coneCryo.xml',
                  'file:Detector/DetFCChhCalDiscs/compact/Forward_coneCryo.xml',
                  'file:Detector/DetFCChhTailCatcher/compact/FCChh_TailCatcher.xml',
                  'file:Detector/DetFCChhBaseline1/compact/FCChh_Solenoids.xml',
                  'file:Detector/DetFCChhBaseline1/compact/FCChh_Shielding.xml' ],
                   OutputLevel = DEBUG)

# Pileup in ECal Barrel
# readout name
ecalBarrelReadoutNamePhiEta = "ECalBarrelPhiEta"
hcalBarrelReadoutNamePhiEta = "BarHCal_Readout_phieta"
ecalEndcapReadoutName = "EMECPhiEtaReco"

# geometry tool
from Configurables import TubeLayerPhiEtaCaloTool
ecalBarrelGeometry = TubeLayerPhiEtaCaloTool("EcalBarrelGeo",
                                             readoutName = ecalBarrelReadoutNamePhiEta,
                                             activeVolumeName = "LAr_sensitive",
                                             activeFieldName = "layer",
                                             fieldNames = ["system"],
                                             fieldValues = [5],
                                             activeVolumesNumber = 8)

hcalBarrelGeometry = TubeLayerPhiEtaCaloTool("HcalBarrelGeo",
                                             readoutName = hcalBarrelReadoutNamePhiEta,
                                             activeVolumeName = "layer",
                                             activeFieldName = "layer",
                                             fieldNames = ["system"],
                                             fieldValues = [8],
                                             activeVolumesNumber = 10)

emecGeometry = TubeLayerPhiEtaCaloTool("emecGeo",
                                       readoutName = ecalEndcapReadoutName,
                                       activeVolumeName = "layerEnvelope",
                                       activeFieldName = "layer",
                                       activeVolumesNumber = 40,
                                       fieldNames = ["system"],
                                       fieldValues = [6])

# call pileup tool
# prepare TH2 histogram with pileup per abs(eta)
from Configurables import PreparePileup
pileupEcalBarrel = PreparePileup("PreparePileupEcalBarrel",
                       geometryTool = ecalBarrelGeometry,
                       readoutName = ecalBarrelReadoutNamePhiEta,
                       layerFieldName = "layer",
                       histogramName = "ecalBarrelEnergyVsAbsEta",
                       numLayers = 8)
pileupEcalBarrel.hits.Path="ECalBarrelCells"

pileupEmec = PreparePileup("PreparePileupEmec",
                       geometryTool = emecGeometry,
                       readoutName = ecalEndcapReadoutName,
                       layerFieldName = "layer",
                       histogramName = "emecEnergyVsAbsEta",
                       numLayers = 40)
pileupEmec.hits.Path="ECalBarrelCells"

pileupHcalBarrel = PreparePileup("PreparePileupHcalBarrel",
                       geometryTool = hcalBarrelGeometry,
                       readoutName = hcalBarrelReadoutNamePhiEta,
                       layerFieldName = "layer",
                       histogramName ="hcalBarrelEnergyVsAbsEta",
                       numLayers = 10)
pileupHcalBarrel.hits.Path="HCalBarrelCells"

THistSvc().Output = ["rec DATAFILE='pileupCalBarrel.root' TYP='ROOT' OPT='RECREATE'"]
THistSvc().PrintAll=True
THistSvc().AutoSave=True
THistSvc().AutoFlush=True
THistSvc().OutputLevel=INFO

#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]
podioinput.AuditExecute = True

ApplicationMgr(
    TopAlg = [podioinput,
              pileupEcalBarrel,
              pileupHcalBarrel,
              pileupEmec
              ],
    EvtSel = 'NONE',
    EvtMax = 10,
    ExtSvc = [podioevent, geoservice],
 )
