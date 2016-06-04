#JANA: variables ENE (energy in MeV!!!!), BFIELD (0,1), EVTMAX (number of events) to be defined before running
ENE = 50000
BFIELD = 0
EVTMAX = 1

from Gaudi.Configuration import *

# Data service
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")

# Magnetic field
from Configurables import G4ConstantMagneticFieldTool
if BFIELD==1:
    field = G4ConstantMagneticFieldTool("G4ConstantMagneticFieldTool",FieldOn=True)
else: 
    field = G4ConstantMagneticFieldTool("G4ConstantMagneticFieldTool",FieldOn=False)

# DD4hep geometry service
# Parses the given xml file
from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[# 'file:DetectorDescription/Detectors/compact/FCChh_DectMaster.xml',
                                          'file:DetectorDescription/Detectors/compact/FCChh_ECalBarrel_Mockup.xml'
                                        ],
                    OutputLevel = INFO)

# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import G4SimSvc, G4SingleParticleGeneratorTool 
# Configures the Geant simulation: geometry, physics list and user actions
geantservice = G4SimSvc("G4SimSvc", detector='G4DD4hepDetector', physicslist="G4FtfpBert", actions="G4FullSimActions")

#Setting random seeds for Geant4 simulations
#Two parameters required (don't know why), Anna suggested to fix the second one to 0 and change only the first one
#x=12768674
#geantservice.G4commands += ["/random/setSeeds "+str(x)+" 0"] #where x is the number you want

# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
from Configurables import G4SimAlg, G4SaveCalHits
# and a tool that saves the calorimeter hits with a name "G4SaveCalHits/saveHCalHits"

saveecaltool = G4SaveCalHits("saveECalHits", caloType = "ECal")
saveecaltool.DataOutputs.caloClusters.Path = "ECalClusters"
saveecaltool.DataOutputs.caloHits.Path = "ECalHits"

# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
pgun=G4SingleParticleGeneratorTool("G4SingleParticleGeneratorTool",
                particleName="e-",energyMin=ENE,energyMax=ENE,etaMin=0.25,etaMax=0.25,
                OutputLevel = INFO)
geantsim = G4SimAlg("G4SimAlg",
                       outputs= ["G4SaveCalHits/saveECalHits"],
                       eventGenerator=pgun)

# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out",
                   OutputLevel=INFO)
out.outputCommands = ["keep *"]
out.filename = "output_test.root"

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [geantsim, out],
                EvtSel = 'NONE',
                EvtMax   = EVTMAX,
                # order is important, as GeoSvc is needed by G4SimSvc
                ExtSvc = [podioevent, geoservice, geantservice],
                OutputLevel=INFO
)
