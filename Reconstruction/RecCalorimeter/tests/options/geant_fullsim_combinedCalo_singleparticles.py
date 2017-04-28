import os
import numpy as np

#loads array of random seeds from file                                                                                                                                                                          
#seed_array = np.loadtxt('/afs/cern.ch/user/c/cneubuse/FCCSW/condor/seeds.txt',dtype='int',delimiter=',')

#set these in the .sh script                                                                                                                                                                                                                
energy=100e3
num_events=10
magnetic_field=0
i=1
particle=1

particleType = "pi-"
if particle==0:
    particleType = "e-"
if particle==2:
    particleType = "mu-"
print particleType

from Gaudi.Configuration import *

from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

podioevent   = FCCDataSvc("EventDataSvc")

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[  'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                           'file:Detector/DetFCChhECalInclined/compact/FCChh_ECalBarrel_withCryostat.xml',
                                          'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalBarrel_TileCal.xml'],
                    OutputLevel = INFO)
# Geant4 service                                                                                                                                                                         
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc", detector='SimG4DD4hepDetector', physicslist="SimG4FtfpBert", actions="SimG4FullSimActions")

# range cut                                                                                                                                                                                                                        
geantservice.G4commands += ["/run/setCut 0.1 mm"]

# Magnetic field                                                                                                                                                                                                                           
from Configurables import SimG4ConstantMagneticFieldTool
field = SimG4ConstantMagneticFieldTool("SimG4ConstantMagneticFieldTool",FieldOn=False)

# Geant4 algorithm                                                                                                                                                                                                       
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools                                                                                                                                          
# and a tool that saves the calorimeter hits                                                                                                                                                                                         
from Configurables import SimG4Alg, SimG4SaveCalHits, InspectHitsCollectionsTool
saveecaltool = SimG4SaveCalHits("saveECalHits", readoutNames = ["ECalHitsEta"],
                                positionedCaloHits = "ECalPositionedHits",
                                caloHits = "ECalHits")
savehcaltool = SimG4SaveCalHits("saveHCalHits",readoutNames = ["BarHCal_Readout"],
                                positionedCaloHits="HCalPositionedHits",
                                caloHits="HCalHits")

# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")                                                                                                                                                     
from Configurables import SimG4SingleParticleGeneratorTool
pgun = SimG4SingleParticleGeneratorTool("SimG4SingleParticleGeneratorTool",saveEdm=True,
                particleName=particleType,energyMin=energy,energyMax=energy,etaMin=0.36,etaMax=0.36,
                OutputLevel =DEBUG)

geantsim = SimG4Alg("SimG4Alg",
                       outputs= ["SimG4SaveCalHits/saveECalHits", "SimG4SaveCalHits/saveHCalHits"],
                       eventProvider=pgun,
                       OutputLevel=INFO)

# common ECAL specific information
# readout name
ecalReadoutName = "ECalHitsEta"
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

# Configure tools for calo reconstruction                                                                                                                                                                    
from Configurables import CalibrateInLayersTool
calibEcells = CalibrateInLayersTool("Calibrate",
                                    # sampling fraction obtained using SamplingFractionInLayers from DetStudies package
                                    samplingFraction = [0.168] * 4 + [0.176] * 4 + [0.184] * 4 + [0.191] * 4 + [0.198] * 4 + [0.204] * 4 + [0.210] * 4 + [0.215] * 4,
                                    readoutName = "ECalHitsEta",
                                    layerFieldName = "cell")

#Configure tools for calo reconstruction
from Configurables import CalibrateCaloHitsTool
calibHcells = CalibrateCaloHitsTool("CalibrateHCal", invSamplingFraction="31")

from Configurables import CreateCaloCells
createEcells = CreateCaloCells("CreateECaloCells",
                               doCellCalibration=True,
                               calibTool=calibEcells,
                               addCellNoise=False, filterCellNoise=False,
                               OutputLevel=INFO,
                               hits="ECalHits",
                               cells="ECalCells")

createHcells = CreateCaloCells("CreateHCaloCells",
                               doCellCalibration=True,
                               calibTool=calibHcells,
                               addCellNoise = False, filterCellNoise = False,
                               OutputLevel = INFO,
                               hits="HCalHits",
                               cells="HCalCells")

# additionally for HCal
from Configurables import CreateVolumeCaloPositions
positions = CreateVolumeCaloPositions("positions", OutputLevel = INFO)
positions.hits.Path = "HCalCells"
positions.positionedHits.Path = "HCalPositions"

from Configurables import RedoSegmentation
resegment = RedoSegmentation("ReSegmentation",
                             # old bitfield (readout)
                             oldReadoutName = hcalReadoutName,
                             # # specify which fields are going to be altered (deleted/rewritten)
                             oldSegmentationIds = ["eta","phi"],
                             # new bitfield (readout), with new segmentation
                             newReadoutName = newHcalReadoutName,
                             debugPrint = 10,
                             OutputLevel = INFO,
                             inhits = "HCalPositions",
                             outhits = "newHCalCells")
# clusters are needed, with deposit position and cellID in bits

positions2 = CreateVolumeCaloPositions("positions2", OutputLevel = INFO)
positions2.hits.Path = "newHCalCells"
positions2.positionedHits.Path = "newHCalPositions"

# Ecal cell positions
positionsEcal = CreateVolumeCaloPositions("positionsEcal", OutputLevel = INFO)
positionsEcal.hits.Path = "ECalCells"
positionsEcal.positionedHits.Path = "ECalPositions"

out = PodioOutput("out", 
                  OutputLevel=DEBUG)
out.outputCommands = ["keep *", "drop ECalHits", "drop HCalHits", "drop HCalCells", "drop HCalPositions", "drop ECalCells", "drop newHCalCells"]
out.filename = "output_combCalo_"+str(particleType)+str(int(energy/1e3))+"GeV_part"+str(i)+".root"

#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]
geantsim.AuditExecute = True
createEcells.AuditExecute = True
createHcells.AuditExecute = True
positions.AuditExecute = True
resegment.AuditExecute = True
positions2.AuditExecute = True
out.AuditExecute = True

ApplicationMgr(
    TopAlg = [geantsim,
              createEcells,
              createHcells,
              positions,
              resegment,
              positions2,
              positionsEcal,
              out
              ],
    EvtSel = 'NONE',
    EvtMax   = int(num_events),
    ExtSvc = [podioevent, geoservice, geantservice, audsvc],
 )

