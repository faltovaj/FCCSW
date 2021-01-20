# variables energy (energy in MeV!!!!), magnetic_field (0,1), num_events (number of events) to be defined before running
energy = 50000
magnetic_field = 0
num_events = 1

from Gaudi.Configuration import *
import sys

os.path.dirname('/afs/cern.ch/work/n/novaj/HEP-FCC/FCCSW')

# Data service
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")

# DD4hep geometry service
from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[ 'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                          'file:Detector/DetFCChhECalSimple/compact/FCChh_ECalBarrel_Mockup.xml'
                                        ],
                    OutputLevel = INFO)

# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc", detector='SimG4DD4hepDetector', physicslist="SimG4FtfpBert", actions="SimG4FullSimActions")


# Magnetic field
from Configurables import SimG4ConstantMagneticFieldTool
if magnetic_field==1:
    field = SimG4ConstantMagneticFieldTool("SimG4ConstantMagneticFieldTool",FieldOn=True,IntegratorStepper="ClassicalRK4")
else:
    field = SimG4ConstantMagneticFieldTool("SimG4ConstantMagneticFieldTool",FieldOn=False)

#Setting random seeds for Geant4 simulations
#geantservice.g4PreInitCommands  += ["/random/setSeeds "+str(x)+" 0"] #where x is the number you want

#range cut
geantservice.g4PreInitCommands += ["/run/setCut 0.1 mm"]

# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
# and a tool that saves the calorimeter hits
from Configurables import SimG4Alg, SimG4SaveCalHits
saveecaltool = SimG4SaveCalHits("saveECalHits",readoutNames = ["ECalHitsPhiEta"])
saveecaltool.positionedCaloHits.Path = "ECalPositionedHits"
saveecaltool.caloHits.Path = "ECalHits"

# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
from Configurables import SimG4SingleParticleGeneratorTool
pgun=SimG4SingleParticleGeneratorTool("SimG4SingleParticleGeneratorTool",saveEdm=True,
                particleName="e-",energyMin=energy,energyMax=energy,etaMin=-1.5,etaMax=1.5,
                OutputLevel =DEBUG)

geantsim = SimG4Alg("SimG4Alg",
                       outputs= ["SimG4SaveCalHits/saveECalHits"],
                       eventProvider=pgun,
                       OutputLevel=DEBUG)

# Note: Optional: merge layers (each 2 layers are merged in this example)
#from Configurables import MergeLayers
#mergeLayers = MergeLayers("MergeLayers",
#                   # take the bitfield description from the geometry service
#                   readout = "ECalHitsPhiEta",
#                   # cells in which field should be merged
#                   identifier = "active_layer",
#                   volumeName = "active_layer",
#                   # how many cells to merge
#                   merge = [2]*50,
#                   OutputLevel = INFO)
#mergeLayers.inhits.Path = "ECalHits"
#mergeLayers.outhits.Path = "mergedECalHits"

# Merge Geant4 hits into cells
# set doCellCalibration to True if you want to apply sampling fraction correction
# Note: replace hits="ECalHits" with hits="mergedECalHits" for merged layers
from Configurables import CreateCaloCells
createCells = CreateCaloCells("CreateCells",
                               doCellCalibration=False,
                               addCellNoise=False, filterCellNoise=False,
                               OutputLevel=INFO,
                               hits="ECalHits",
                               cells="ECalCells")

# Add positions to the cell
# Single layer thickness - active + passive is 0.6 cm in the default geometry
# Note: change layerThickness to 1.2 (cm) for merged layers in the example
from Configurables import CellPositionsCaloSimpleTool
caloSimplePosition = CellPositionsCaloSimpleTool("CaloSimplePosition",
                                                 readoutName = "ECalHitsPhiEta",
                                                 rminFirstLayer = 195.,
                                                 layerThickness = 0.6,
                                                 OutputLevel=DEBUG)

from Configurables import CreateCellPositions
cellPositions = CreateCellPositions("CellPositions",
                                     positionsTool = caloSimplePosition,
                                     hits = "ECalCells",
                                     positionedHits = "ECalCellsPositions")

# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out",
                   OutputLevel=DEBUG)
out.outputCommands = ["keep *"]
out.filename = "output_ecalSim_e50GeV_"+str(num_events)+"events.root"

#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]
geantsim.AuditExecute = True

# ApplicationMgr
# Note: For merged layers add mergeLayers in the TopAlgs before createCells
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [geantsim, createCells, cellPositions, out],
                EvtSel = 'NONE',
                EvtMax   = num_events,
                # order is important, as GeoSvc is needed by G4SimSvc
                ExtSvc = [podioevent, geoservice, geantservice, audsvc],
                OutputLevel=DEBUG
)