#JANA: variables ENE (energy in MeV!!!!), BFIELD (0,1), EVTMAX (number of events) to be defined before running
ENE = 50000
BFIELD = 0
EVTMAX = 100

from Gaudi.Configuration import *

# Data service
from Configurables import FCCDataSvc
podioevent = FCCDataSvc("EventDataSvc")

# DD4hep geometry service
# Parses the given xml file
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
if BFIELD==1:
    field = SimG4ConstantMagneticFieldTool("SimG4ConstantMagneticFieldTool",FieldOn=True,IntegratorStepper="ClassicalRK4")    
else: 
    field = SimG4ConstantMagneticFieldTool("SimG4ConstantMagneticFieldTool",FieldOn=False)

#Setting random seeds for Geant4 simulations
#Two parameters required (don't know why), Anna suggested to fix the second one to 0 and change only the first one
#x=12768674
#geantservice.G4commands += ["/random/setSeeds "+str(x)+" 0"] #where x is the number you want

#range cut
geantservice.G4commands += ["/run/setCut 0.1 mm"]

# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
from Configurables import SimG4Alg, SimG4SaveCalHits
# and a tool that saves the calorimeter hits with a name "G4SaveCalHits/saveHCalHits"

saveecaltool = SimG4SaveCalHits("saveECalHits",readoutNames = ["ECalHitsPhiEta"])
saveecaltool.DataOutputs.caloClusters.Path = "Event/ECalClusters"
saveecaltool.DataOutputs.caloHits.Path = "Event/ECalHits"

# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
from Configurables import SimG4SingleParticleGeneratorTool
pgun=SimG4SingleParticleGeneratorTool("SimG4SingleParticleGeneratorTool",saveEdm=True,
                particleName="e-",energyMin=ENE,energyMax=ENE,etaMin=-0.01,etaMax=0.01,
                OutputLevel =DEBUG)
#Following lines do not work, no idea why:
#pgun.DataOutputs.genParticles.Path = "genParticles"
#pgun.DataOutputs.genVertices.Path="genVertices"

geantsim = SimG4Alg("SimG4Alg",
                       outputs= ["SimG4SaveCalHits/saveECalHits"],
                       eventProvider=pgun,
                       OutputLevel=DEBUG)

# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out",
                   OutputLevel=INFO)
out.outputCommands = ["keep *"]
out.filename = "output_ecal.root"

#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]
geantsim.AuditExecute = True

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [geantsim, out],
                EvtSel = 'NONE',
                EvtMax   = EVTMAX,
                # order is important, as GeoSvc is needed by G4SimSvc
                ExtSvc = [podioevent, geoservice, geantservice, audsvc],
                OutputLevel=DEBUG
)