from Gaudi.Configuration import *

# DD4hep geometry service
from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[ 'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                          'file:Detector/DetFCChhECalInclined/compact/FCChh_ECalBarrel_withCryostat.xml',
                                          'file:Detector/DetFCChhHCalTile/compact/FCChh_HCalBarrel_TileCal.xml'
                                        ],
                    OutputLevel = INFO)

# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import CreateFCChhCaloNeighbours
neighbours = CreateFCChhCaloNeighbours("neighbours", 
                                       outputFileName="cellNeighbours_Barrel.root",
                                       connectBarrels=True, 
                                       readoutNamesVolumes=["HCalBarrelReadout"],
                                       systemNameNested="system",
                                       systemValuesNested=[8],
                                       activeFieldNamesNested=["layer","module","row"],
                                       hCalPhiOffset = -3.12932,
                                       hCalRinner=2850,
                                       OutputLevel=DEBUG)

# ApplicationMgr
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [],
                EvtSel = 'NONE',
                EvtMax   = 1,
                # order is important, as GeoSvc is needed by G4SimSvc
                ExtSvc = [geoservice, neighbours],
                OutputLevel=INFO
)
