from Gaudi.Configuration import *

from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

# v01 production - min. bias events
podioevent = FCCDataSvc("EventDataSvc", input="/eos/experiment/fcc/hh/simulation/samples/v01/physics/MinBias/bFieldOn/etaFull/simu/output_condor_novaj_201801081606412074.root")

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
 #                 'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalExtendedBarrel_TileCal.xml',
                  'file:Detector/DetFCChhCalDiscs/compact/Endcaps_coneCryo.xml',
                  'file:Detector/DetFCChhCalDiscs/compact/Forward_coneCryo.xml',
                  'file:Detector/DetFCChhTailCatcher/compact/FCChh_TailCatcher.xml',
                  'file:Detector/DetFCChhBaseline1/compact/FCChh_Solenoids.xml',
                  'file:Detector/DetFCChhBaseline1/compact/FCChh_Shielding.xml' ],
                    OutputLevel = INFO)

# Pileup in ECal Barrel
# readout name
ecalBarrelReadoutNamePhiEta = "ECalBarrelPhiEta"
hcalBarrelReadoutName = "BarHCal_Readout"
hcalBarrelReadoutVolume = "HCalBarrelReadout"
# HCal Barrel
# active material identifier name
hcalIdentifierName = ["module", "row", "layer"]
# active material volume name
hcalVolumeName = ["moduleVolume", "wedgeVolume", "layerVolume"]
# ECAL bitfield names & values
hcalFieldNames=["system"]
hcalFieldValues=[8]


#Configure tools for calo cell positions
from Configurables import CellPositionsECalBarrelTool, CellPositionsHCalBarrelTool, CellPositionsHCalBarrelNoSegTool, CellPositionsCaloDiscsTool, CellPositionsTailCatcherTool 
ECalBcells = CellPositionsECalBarrelTool("CellPositionsECalBarrel", 
                                         readoutName = ecalBarrelReadoutNamePhiEta, 
                                         OutputLevel = INFO)
#EMECcells = CellPositionsCaloDiscsTool("CellPositionsEMEC", 
#                                       readoutName = ecalEndcapReadoutName, 
#                                       OutputLevel = INFO)
#ECalFwdcells = CellPositionsCaloDiscsTool("CellPositionsECalFwd", 
#                                          readoutName = ecalFwdReadoutName, 
#                                          OutputLevel = INFO)
HCalBcellVols = CellPositionsHCalBarrelNoSegTool("CellPositionsHCalBarrelVols", 
                                                 readoutName = hcalBarrelReadoutVolume, 
                                                 OutputLevel = INFO)
#HCalExtBcellVols = CellPositionsHCalBarrelNoSegTool("CellPositionsHCalExtBarrel", 
#                                                    readoutName = hcalExtBarrelReadoutVolume, 
#                                                    OutputLevel = INFO)
#HECcells = CellPositionsCaloDiscsTool("CellPositionsHEC", 
#                                      readoutName = hcalEndcapReadoutName, 
#                                      OutputLevel = INFO)
#HCalFwdcells = CellPositionsCaloDiscsTool("CellPositionsHCalFwd", 
#                                          readoutName = hcalFwdReadoutName, 
#                                          OutputLevel = INFO)


from Configurables import RewriteHCalBarrelBitfield
rewriteHcal = RewriteHCalBarrelBitfield("RewriteHCalBitfield", 
                                        oldReadoutName = hcalBarrelReadoutName,
                                        newReadoutName = hcalBarrelReadoutVolume,
                                        # specify if segmentation is removed                                                                                                                                                  
                                        removeIds = ["tile","eta","phi"],
                                        debugPrint = 10,
                                        OutputLevel = INFO)
# clusters are needed, with deposit position and cellID in bits                                                                                                                                                                           
rewriteHcal.inhits.Path = "HCalBarrelCells"
rewriteHcal.outhits.Path = "HCalBarrelCellsVol"

# geometry tool
from Configurables import TubeLayerPhiEtaCaloTool
ecalBarrelGeometry = TubeLayerPhiEtaCaloTool("EcalBarrelGeo",
                                             readoutName = ecalBarrelReadoutNamePhiEta,
                                             activeVolumeName = "LAr_sensitive",
                                             activeFieldName = "layer",
                                             fieldNames = ["system"],
                                             fieldValues = [5],
                                             activeVolumesNumber = 8)

from Configurables import NestedVolumesCaloTool
hcalBarrelGeometry = NestedVolumesCaloTool("HcalBarrelGeo",
                                           activeVolumeName = hcalVolumeName,
                                           activeFieldName = hcalIdentifierName,
                                           readoutName = hcalBarrelReadoutVolume,
                                           fieldNames = hcalFieldNames,
                                           fieldValues = hcalFieldValues,
                                           OutputLevel = INFO)
# call pileup tool
# prepare TH2 histogram with pileup per abs(eta)
from Configurables import PreparePileup
pileupEcalBarrel = PreparePileup("PreparePileupEcalBarrel",
                                 geometryTool = ecalBarrelGeometry,
                                 positionsTool = ECalBcells,
                                 readoutName = ecalBarrelReadoutNamePhiEta,
                                 layerFieldName = "layer",
                                 histogramName = "ecalBarrelEnergyVsAbsEta",
                                 numLayers = 8,
                                 OutputLevel = DEBUG)
pileupEcalBarrel.hits.Path="ECalBarrelCells"

from Configurables import PreparePileup
pileupHcalBarrel = PreparePileup("PreparePileupHcalBarrel",
                                 geometryTool = hcalBarrelGeometry,
                                 positionsTool = HCalBcellVols,
                                 readoutName = hcalBarrelReadoutVolume,
                                 layerFieldName = "layer",
                                 histogramName ="hcalBarrelEnergyVsAbsEta",
                                 numLayers = 10,
                                 OutputLevel = DEBUG)
pileupHcalBarrel.hits.Path="HCalBarrelCellsVol"

THistSvc().Output = ["rec DATAFILE='pileupCalBarrel.root' TYP='ROOT' OPT='RECREATE'"]
THistSvc().PrintAll=True
THistSvc().AutoSave=True
THistSvc().AutoFlush=True
THistSvc().OutputLevel=INFO

#out = PodioOutput("output", filename = "output.root",
#                   OutputLevel = DEBUG)
#out.outputCommands = ["keep *"]

#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]
podioinput.AuditExecute = True
#pileupEcalBarrel.AuditExecute = True
#out.AuditExecute = True

ApplicationMgr(
    TopAlg = [podioinput,
              rewriteHcal,
              pileupEcalBarrel,
              pileupHcalBarrel,
              ],
    EvtSel = 'NONE',
    EvtMax = 10,
    ExtSvc = [podioevent, geoservice],
 )
