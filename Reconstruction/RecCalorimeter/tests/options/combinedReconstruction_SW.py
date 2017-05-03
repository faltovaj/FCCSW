num_events = 10
energy = 50
particleType = "e"

from Gaudi.Configuration import *

from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

podioevent = FCCDataSvc("EventDataSvc", input="output_combCalo_e-50GeV_part1.root")

# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import PodioInput
podioinput = PodioInput("PodioReader", collections = ["ECalPositions", "newHCalPositions", "GenParticles"], OutputLevel = DEBUG)

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[  'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                           'file:Detector/DetFCChhECalInclined/compact/FCChh_ECalBarrel_withCryostat.xml',
                                          'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalBarrel_TileCal.xml'],
                    OutputLevel = INFO)

# common ECAL specific information
# readout name
ecalReadoutName = "ECalHitsEta"
ecalReadoutNameOffset = "ECalHitsPhiEta_offset"
# active material identifier name
ecalIdentifierName = "active_layer"
# active material volume name
ecalVolumeName = "LAr_sensitive"
# ECAL bitfield names & values
ecalFieldNames=["system","ECAL_Cryo","bath","EM_barrel"]
ecalFieldValues=[5,1,1,1]
# common HCAL specific information
# readout name
hcalReadoutName = "BarHCal_Readout"
# active material identifier name
hcalIdentifierName = ["module","row","layer","tile"]
# active material volume name
hcalVolumeName = ["module","wedge","layer","tile"]
# ECAL bitfield names & values
hcalFieldNames=["system"]
# readout name
newHcalReadoutName = hcalReadoutName + "_phieta"
hcalReadoutNameOffset = hcalReadoutName + "_phieta" + "_offset"

# Configure tools for calo reconstruction                                                                                                

from Configurables import RedoSegmentation
resegmentEcal = RedoSegmentation("ReSegmentationEcal",
                             # old bitfield (readout)
                             oldReadoutName = ecalReadoutName,
                             # # specify which fields are going to be altered (deleted/rewritten)
                             oldSegmentationIds = ["eta","phi"],
                             # new bitfield (readout), with new segmentation
                             newReadoutName = ecalReadoutNameOffset,
                             OutputLevel = INFO,
                             inhits = "ECalPositions",
                             outhits = "ECalCellsForSW")

resegmentHcal = RedoSegmentation("ReSegmentationHcal",
                             # old bitfield (readout)
                             oldReadoutName = newHcalReadoutName,
                             # # specify which fields are going to be altered (deleted/rewritten)
                             oldSegmentationIds = ["eta","phi"],
                             # new bitfield (readout), with new segmentation
                             newReadoutName = hcalReadoutNameOffset,
                             OutputLevel = INFO,
                             inhits = "newHCalPositions",
                             outhits = "HCalCellsForSW")


#Create calo clusters
from Configurables import CreateCaloClustersSlidingWindow, SingleCaloTowerTool, CombinedCaloTowerTool
from GaudiKernel.PhysicalConstants import pi
towersEcal = SingleCaloTowerTool("towersEcal",
                             deltaEtaTower = 0.01, deltaPhiTower = 2*pi/524.,
                             readoutName = ecalReadoutNameOffset,
                             OutputLevel = DEBUG)
towersEcal.cells.Path = "ECalCellsForSW"

towers = CombinedCaloTowerTool("towers",
                             deltaEtaTower = 0.01, deltaPhiTower = 2*pi/524.,
                             ecalReadoutName = ecalReadoutNameOffset,
                             hcalReadoutName = hcalReadoutNameOffset,
                             OutputLevel = DEBUG)
towers.ecalCells.Path = "ECalCellsForSW"
towers.hcalCells.Path = "HCalCellsForSW"


createEcalClusters = CreateCaloClustersSlidingWindow("CreateEcalClusters",
                                                 towerTool = towersEcal,
                                                 nEtaWindow = 7, nPhiWindow = 15,
                                                 nEtaPosition = 5, nPhiPosition = 11,
                                                 nEtaDuplicates = 5, nPhiDuplicates = 11,
                                                 nEtaFinal = 7, nPhiFinal = 15,
                                                 energyThreshold = 8,
                                                 OutputLevel = DEBUG)
createEcalClusters.clusters.Path = "EcalClusters"

createCombinedClusters = CreateCaloClustersSlidingWindow("CreateCombinedClusters",
                                                 towerTool = towers,
                                                 nEtaWindow = 7, nPhiWindow = 15,
                                                 nEtaPosition = 5, nPhiPosition = 11,
                                                 nEtaDuplicates = 5, nPhiDuplicates = 11,
                                                 nEtaFinal = 7, nPhiFinal = 15,
                                                 energyThreshold = 8,
                                                 OutputLevel = DEBUG)
createCombinedClusters.clusters.Path = "CombinedClusters"

out = PodioOutput("out", filename = "output_combCalo_reconstructionSW_"+particleType+"-"+str(energy)+"GeV_"+str(num_events)+"events.root",
                  OutputLevel=DEBUG)
out.outputCommands = ["keep *","drop ECalHits", "drop HCalHits"]

#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]
resegmentEcal.AuditExecute = True
resegmentHcal.AuditExecute = True
out.AuditExecute = True

ApplicationMgr(
    TopAlg = [podioinput,
              resegmentEcal,
              resegmentHcal,
              createEcalClusters,
              createCombinedClusters,
              out
              ],
    EvtSel = 'NONE',
    EvtMax   = int(num_events),
    ExtSvc = [podioevent, audsvc],
 )
