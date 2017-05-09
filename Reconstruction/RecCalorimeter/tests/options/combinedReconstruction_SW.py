num_events = 500
energy=500
parti=19

from Gaudi.Configuration import *

from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

#podioevent = FCCDataSvc("EventDataSvc", input="/localscratch3/novaj/FCCSW/caloForBerlin/output_combCalo_e"+str(energy)+"GeV_part"+str(parti)+"_v3.root")
podioevent = FCCDataSvc("EventDataSvc", input="root://eospublic//eos/fcc/users/n/novaj/combCaloForBerlin/output_combCalo_e"+str(energy)+"GeV_part"+str(parti)+"_v3.root")

# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import PodioInput
podioinput = PodioInput("PodioReader", collections = ["ECalPositions", "newHCalCells", "GenParticles"], OutputLevel = DEBUG)

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[  'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                           'file:Detector/DetFCChhECalInclined/compact/FCChh_ECalBarrel_withCryostat.xml',
                                          'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalBarrel_TileCal.xml'],
                    OutputLevel = INFO)

# common ECAL specific information
# readout name
ecalReadoutName = "ECalHitsEta"
ecalReadoutNameOffset = "ECalHitsPhiEta"
# active material identifier name
ecalIdentifierName = "active_layer"
# active material volume name
ecalVolumeName = "LAr_sensitive"
# ECAL bitfield names & values
ecalFieldNames=["system","ECAL_Cryo","bath","EM_barrel"]
ecalFieldValues=[5,1,1,1]
# common HCAL specific information
# active material identifier name
#hcalIdentifierName = ["module","row","layer","tile"]
# active material volume name
#hcalVolumeName = ["module","wedge","layer","tile"]
# HCAL bitfield names & values
#hcalFieldNames=["system"]
# readout name
hcalReadoutNameOffset = "BarHCal_Readout_phieta"

# Configure tools for calo reconstruction                                                                                                

from Configurables import RedoSegmentation
resegmentEcal = RedoSegmentation("ReSegmentationEcal",
                             # old bitfield (readout)
                             oldReadoutName = ecalReadoutName,
                             # # specify which fields are going to be altered (deleted/rewritten)
                             oldSegmentationIds = ["module"],
                             # new bitfield (readout), with new segmentation
                             newReadoutName = ecalReadoutNameOffset,
                             OutputLevel = INFO,
                             inhits = "ECalPositions",
                             outhits = "ECalCellsForSW")


#Create calo clusters
from Configurables import CreateCaloClustersSlidingWindow, SingleCaloTowerTool, CombinedCaloTowerTool
from GaudiKernel.PhysicalConstants import pi
towersEcal = SingleCaloTowerTool("towersEcal",
                             deltaEtaTower = 0.01, deltaPhiTower = 2*pi/512.,
                             readoutName = ecalReadoutNameOffset,
                             OutputLevel = DEBUG)
towersEcal.cells.Path = "ECalCellsForSW"

towers = CombinedCaloTowerTool("towers",
                             deltaEtaTower = 0.01, deltaPhiTower = 2*pi/512.,
                             ecalReadoutName = ecalReadoutNameOffset,
                             hcalReadoutName = hcalReadoutNameOffset,
                             OutputLevel = DEBUG)
towers.ecalCells.Path = "ECalCellsForSW"
towers.hcalCells.Path = "newHCalCells"


createEcalClusters = CreateCaloClustersSlidingWindow("CreateEcalClusters",
                                                 towerTool = towersEcal,
                                                 nEtaWindow = 13, nPhiWindow = 13,
                                                 nEtaPosition = 7, nPhiPosition = 7,
                                                 nEtaDuplicates = 5, nPhiDuplicates = 5,
                                                 nEtaFinal = 15, nPhiFinal = 19,
                                                 energyThreshold = 10,
                                                 OutputLevel = DEBUG)
createEcalClusters.clusters.Path = "EcalClusters"

createCombinedClusters = CreateCaloClustersSlidingWindow("CreateCombinedClusters",
                                                 towerTool = towers,
                                                 nEtaWindow = 13, nPhiWindow = 13,
                                                 nEtaPosition = 7, nPhiPosition = 7,
                                                 nEtaDuplicates = 5, nPhiDuplicates = 5,
                                                 nEtaFinal = 15, nPhiFinal = 19,
                                                 energyThreshold = 10,
                                                 OutputLevel = DEBUG)
createCombinedClusters.clusters.Path = "CombinedClusters"

out = PodioOutput("out", filename = "/localscratch3/novaj/FCCSW/caloForBerlin/output_combCalo_reconstructionSW_e"+str(energy)+"GeV_part"+str(parti)+".root",
                  OutputLevel=DEBUG)
out.outputCommands = ["keep *","drop ECalHits", "drop HCalHits"]

#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]
resegmentEcal.AuditExecute = True
createEcalClusters.AuditExecute = True
createCombinedClusters.AuditExecute= True
out.AuditExecute = True

ApplicationMgr(
    TopAlg = [podioinput,
              resegmentEcal,
              createEcalClusters,
              createCombinedClusters,
              out
              ],
    EvtSel = 'NONE',
    EvtMax   = int(num_events),
    ExtSvc = [geoservice, podioevent, audsvc],
 )
