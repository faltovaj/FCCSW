#JANA: variables ENE (energy in MeV!!!!), BFIELD (0,1), EVTMAX (number of events) to be defined before running
ENE = 50000
BFIELD = 0
EVTMAX = 2

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
geoservice = GeoSvc("GeoSvc", detectors=[ 'file:DetectorDescription/Detectors/compact/FCChh_DectMaster.xml',
                                          'file:DetectorDescription/Detectors/compact/FCChh_ECalBarrel_Mockup.xml'
                                        ],
                    OutputLevel = INFO)

# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import G4SimSvc, G4SingleParticleGeneratorTool 
# Configures the Geant simulation: geometry, physics list and user actions
geantservice = G4SimSvc("G4SimSvc", detector='G4DD4hepDetector', physicslist="G4FtfpBert",
particleGenerator=G4SingleParticleGeneratorTool("G4SingleParticleGeneratorTool",
                ParticleName="e-",eMin=ENE,eMax=ENE,etaMin=0.25,etaMax=0.25,phiMin=0.0,phiMax=3.14,VertexX=0,VertexY=2600,VertexZ=0),
#                ParticleName="e-",eMin=ENE,eMax=ENE,etaMin=0.25,etaMax=0.25),
                actions="G4FullSimActions") 

# reads an HepMC::GenEvent from the data service and writes a collection of EDM Particles
#from Configurables import HepMCConverter
#hepmc_converter = HepMCConverter("Converter")
#hepmc_converter.DataInputs.hepmc.Path="hepmc"
#hepmc_converter.DataOutputs.genparticles.Path="allGenParticles"
#hepmc_converter.DataOutputs.genvertices.Path="allGenVertices"

 
# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
from Configurables import G4SimAlg, G4SaveCalHits, G4SaveMCTruthParticles
# and a tool that saves the calorimeter hits with a name "G4SaveCalHits/saveHCalHits"

saveecaltool = G4SaveCalHits("saveECalHits", caloType = "ECal")
saveecaltool.DataOutputs.caloClusters.Path = "ECalClusters"
saveecaltool.DataOutputs.caloHits.Path = "ECalHits"

savemcparticlestool = G4SaveMCTruthParticles("saveMCTruthParticles")
savemcparticlestool.DataOutputs.particlesMC.Path = "MCTruthParticles"
savemcparticlestool.DataOutputs.vertices.Path = "GenVertices"

# next, create the G4 algorithm, giving the list of names of tools ("XX/YY")
geantsim = G4SimAlg("G4SimAlg",
                       outputs= ["G4SaveCalHits/saveECalHits", "G4SaveMCTruthParticles/saveMCTruthParticles"])
#                    outputs= ["G4SaveCalHits/saveECalHits"])

geantsim.DataInputs.genParticles.Path="allGenParticles"

# PODIO algorithm
from Configurables import PodioOutput
out = PodioOutput("out",
                   OutputLevel=INFO)
out.outputCommands = ["keep *"]

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( #TopAlg = [hepmc_converter, geantsim, out],
                TopAlg = [geantsim, out],
                EvtSel = 'NONE',
                EvtMax   = EVTMAX,
                # order is important, as GeoSvc is needed by G4SimSvc
                ExtSvc = [podioevent, geoservice, geantservice],
                OutputLevel=INFO
)
