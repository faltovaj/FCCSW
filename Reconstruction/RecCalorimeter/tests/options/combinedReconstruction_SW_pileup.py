num_events = 500
energy=20
parti=3
bfield=1
suffix="_eta0_v2"
suffix2="_eta0_pileup"

from Gaudi.Configuration import *

from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

#podioevent = FCCDataSvc("EventDataSvc", input="/localscratch3/novaj/FCCSW/caloForBerlin/output_combCalo_e"+str(energy)+"GeV_part"+str(parti)+"_v3.root")
podioevent = FCCDataSvc("EventDataSvc", input="root://eospublic//eos/fcc/users/n/novaj/combCaloForBerlin/output_combCalo_e"+str(energy)+"GeV_bfield"+str(bfield)+"_part"+str(parti)+suffix+".root")

# reads HepMC text file and write the HepMC::GenEvent to the data service
from Configurables import PodioInput
podioinput = PodioInput("PodioReader", collections = ["ECalPositions", "newHCalCells", "GenParticles", "GenVertices"], OutputLevel = DEBUG)

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[  'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                           'file:Detector/DetFCChhECalInclined/compact/FCChh_ECalBarrel_withCryostat.xml',
                                          'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalBarrel_TileCal.xml'],
                    OutputLevel = DEBUG)

# common ECAL specific information
# readout name
ecalReadoutName = "ECalHitsEta"
ecalReadoutNamePhiEta = "ECalHitsPhiEta"
# active material identifier name
ecalIdentifierName = "cell"
# active material volume name
ecalVolumeName = "cell"
# ECAL bitfield names & values
ecalFieldNames=["system","cryo","type","subtype"]
ecalFieldValues=[5,0,0,0]
# Number of layers to be merged in each layer
ecalNumberOfLayersToMerge=[5] + [4] * 7
# number of ECAL layers
ecalNumberOfLayers = len(ecalNumberOfLayersToMerge)
# common HCAL specific information
# readout name
hcalReadoutNamePhiEta = "BarHCal_Readout_phieta"

# Prepare ecal cells
# Resegment to phi-eta grid
from Configurables import RedoSegmentation
resegmentEcal = RedoSegmentation("ReSegmentationEcal",
                             # old bitfield (readout)
                             oldReadoutName = ecalReadoutName,
                             # # specify which fields are going to be altered (deleted/rewritten)
                             oldSegmentationIds = ["module"],
                             # new bitfield (readout), with new segmentation
                             newReadoutName = ecalReadoutNamePhiEta,
                             OutputLevel = INFO,
                             inhits = "ECalPositions",
                             outhits = "ECalCellsStep1")

# Merge layers
from Configurables import MergeLayers
mergeLayersEcal = MergeLayers("MergeLayersEcal",
                   # take the bitfield description from the geometry service
                   readout = ecalReadoutNamePhiEta,
                   # cells in which field should be merged
                   identifier = ecalIdentifierName,
                   volumeName = ecalVolumeName,
                   # how many cells to merge
                   merge = ecalNumberOfLayersToMerge,
                   OutputLevel = INFO)
mergeLayersEcal.inhits.Path = "ECalCellsStep1"
mergeLayersEcal.outhits.Path = "ECalCellsStep2"

#Configure tools for calo reconstruction
from Configurables import CalibrateInLayersTool, NoiseCaloCellsFromFileTool, TubeLayerPhiEtaCaloTool
calibEcells = CalibrateInLayersTool("Calibrate",
                                    # sampling fraction obtained using SamplingFractionInLayers from DetStudies package
                                    samplingFraction = [0.1645/0.168, 0.1736/0.176, 0.179/0.184, 0.1837/0.191, 0.1877/0.198, 0.1919/0.204, 0.1958/0.210, 0.1982/0.215],
                                    readoutName = ecalReadoutNamePhiEta,
                                    layerFieldName = "cell")
noise = NoiseCaloCellsFromFileTool("NoiseCaloCellsFromFileTool",
                                   #noiseFileName="root://eospublic.cern.ch//eos/fcc/users/n/novaj/combCaloForBerlin/EcalPileup_mu1000.root",
                                   noiseFileName="~novaj/public/EcalPileup_Berlin_mu1000.root",
                                   numRadialLayers = 8,
                                   activeFieldName = "cell",
                                   addElecNoise = False);
ecalgeo = TubeLayerPhiEtaCaloTool("EcalGeo",
                                  readoutName = ecalReadoutNamePhiEta,
                                  activeVolumeName = ecalVolumeName,
                                  activeFieldName = ecalIdentifierName,
                                  fieldNames = ecalFieldNames,
                                  fieldValues = ecalFieldValues,
                                  activeVolumesNumber = ecalNumberOfLayers,
                                  OutputLevel = DEBUG)

from Configurables import CreateCaloCells
createCellsEcal = CreateCaloCells("CreateCaloCellsEcal",
                              geometryTool = ecalgeo,
                              doCellCalibration = True,
                              calibTool = calibEcells,
                              addCellNoise = True, filterCellNoise = False,
                              noiseTool = noise,
                              OutputLevel = DEBUG)
createCellsEcal.hits.Path = "ECalCellsStep2"
createCellsEcal.cells.Path = "ECalCellsForSW"

#Create calo clusters
from Configurables import CreateCaloClustersSlidingWindow, SingleCaloTowerTool, CombinedCaloTowerTool
from GaudiKernel.PhysicalConstants import pi

towersEcal = SingleCaloTowerTool("towersEcal",
                             deltaEtaTower = 0.01, deltaPhiTower = 2*pi/512.,
                             readoutName = ecalReadoutNamePhiEta,
                             OutputLevel = DEBUG)
towersEcal.cells.Path = "ECalCellsForSW"


towers = CombinedCaloTowerTool("towers",
                             deltaEtaTower = 0.01, deltaPhiTower = 2*pi/512.,
                             ecalReadoutName = ecalReadoutNamePhiEta,
                             hcalReadoutName = hcalReadoutNamePhiEta,
                             OutputLevel = DEBUG)
towers.ecalCells.Path = "ECalCellsForSW"
towers.hcalCells.Path = "newHCalCells"

windE = 9
windP = 17
posE = 5
posP = 11
dupE = 7
dupP = 13
finE = 9
finP = 17
threshold = 12
createEcalClusters = CreateCaloClustersSlidingWindow("CreateEcalClusters",
                                                 towerTool = towersEcal,
                                                 nEtaWindow = windE, nPhiWindow = windP,
                                                 nEtaPosition = posE, nPhiPosition = posP,
                                                 nEtaDuplicates = dupE, nPhiDuplicates = dupP,
                                                 nEtaFinal = finE, nPhiFinal = finP,
                                                 energyThreshold = threshold,
                                                 OutputLevel = DEBUG)
createEcalClusters.clusters.Path = "EcalClusters"

createCombinedClusters = CreateCaloClustersSlidingWindow("CreateCombinedClusters",
                                                 towerTool = towers,
                                                 nEtaWindow = windE, nPhiWindow = windP,
                                                 nEtaPosition = posE, nPhiPosition = posP,
                                                 nEtaDuplicates = dupE, nPhiDuplicates = dupP,
                                                 nEtaFinal = finE, nPhiFinal = finP,
                                                 energyThreshold = threshold,
                                                 OutputLevel = DEBUG)
createCombinedClusters.clusters.Path = "CombinedClusters"

out = PodioOutput("out", filename = "/tmp/novaj/output_combCalo_reconstructionSW_e"+str(energy)+"GeV_bfield"+str(bfield)+"_part"+str(parti)+suffix2+".root",                   OutputLevel=DEBUG)
out.outputCommands = ["keep *","drop ECalHits", "drop HCalHits", "drop ECalPositions", "drop ECalCellsStep1", "drop ECalCellsStep2"]

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
              mergeLayersEcal,
              createCellsEcal,
              createEcalClusters,
              createCombinedClusters,
              out
              ],
    EvtSel = 'NONE',
    EvtMax   = int(num_events),
    ExtSvc = [geoservice, podioevent, audsvc],
 )
