#include "G4SaveMCTruthParticles.h"

// FCCSW
#include "SimG4Common/Units.h"
#include "SimG4Common/MCTruthEventInformation.h"

// Geant4
#include "G4Event.hh"

// datamodel
#include "datamodel/MCParticleCollection.h"
#include "datamodel/GenVertexCollection.h"

// DD4hep
#include "DDG4/Geant4Hits.h"

DECLARE_TOOL_FACTORY(G4SaveMCTruthParticles)

G4SaveMCTruthParticles::G4SaveMCTruthParticles(const std::string& aType, const std::string& aName, const IInterface* aParent) :
  AlgTool(aType, aName, aParent) {
  declareInterface<IG4SaveOutputTool>(this);
  declareOutput("particlesMC", m_particles,"particles/MCParticles");
  declareOutput("vertices", m_vertices,"vertices/GenVertices");
  // needed for AlgTool wit output/input until it appears in Gaudi AlgTool constructor
  declareProperty("DataInputs", inputDataObjects());
  declareProperty("DataOutputs", outputDataObjects());
}

G4SaveMCTruthParticles::~G4SaveMCTruthParticles() {}

StatusCode G4SaveMCTruthParticles::initialize() {
  return AlgTool::initialize();
}

StatusCode G4SaveMCTruthParticles::finalize() {
  return AlgTool::finalize();
}

StatusCode G4SaveMCTruthParticles::saveOutput(const G4Event& aEvent) {

  auto particles = m_particles.createAndPut();

  sim::MCTruthEventInformation* mcevinf = (sim::MCTruthEventInformation*) aEvent.GetUserInformation();

  /* 
  G4TrajectoryContainer* g4trajectorycontainer = aEvent.GetTrajectoryContainer();

  std::cout << "G4SaveMCTruthParticles:saveOutput, size of the TrajectoryContainer " << g4trajectorycontainer->size() << std::endl;

  for (unsigned int i = 0; i<g4trajectorycontainer->size(); i++) {
    //std::cout << "G4SaveMCTruthParticles: index " << i <<std::endl;
    MCTrajectory* g4trajectory = dynamic_cast<MCTrajectory*>((*g4trajectorycontainer)[i]);
  
    
    fcc::MCParticle particle = particles->create();
    fcc::BareParticle& core = particle.Core();
    core.Type = g4trajectory->GetPDGEncoding();
    core.Status = 1; // how it is defined ???? as in HepMC ?
    core.Charge = g4trajectory->GetCharge();
    core.P4.Px = g4trajectory->GetMomentum().x()*sim::g42edm::energy;
    core.P4.Py = g4trajectory->GetMomentum().y()*sim::g42edm::energy;
    core.P4.Pz = g4trajectory->GetMomentum().z()*sim::g42edm::energy;
    core.P4.Mass = g4trajectory->GetMass()*sim::g42edm::energy;
    core.Vertex.X = g4trajectory->GetVertexPosition().x()*sim::g42edm::length;
    core.Vertex.Y = g4trajectory->GetVertexPosition().y()*sim::g42edm::length;
    core.Vertex.Z = g4trajectory->GetVertexPosition().z()*sim::g42edm::length;
    g4trajectory->ShowTrajectory();
    std::cout << "Conversion factor g42edm::energy " << sim::g42edm::energy << std::endl;
    std::cout << "G4SaveMCTruthParticles: Track ID " << g4trajectory->GetTrackID() << " Parent ID " << g4trajectory->GetParentID() << " Momentum x " <<g4trajectory->GetMomentum().x() << " y " << g4trajectory->GetMomentum().y() << " z " << g4trajectory->GetMomentum().z() << " E " << sqrt(pow(g4trajectory->GetMomentum().x(),2)+pow(g4trajectory->GetMomentum().y(),2)+pow(g4trajectory->GetMomentum().z(),2)) << std::endl;
    std::cout << "G4SaveMCTruthParticles: Point x " << g4trajectory->GetVertexPosition().x() << " y " << g4trajectory->GetVertexPosition().y() << " z " << g4trajectory->GetVertexPosition().z() <<  " mass " << g4trajectory->GetMass() << std::endl;
   }
    */

  return StatusCode::SUCCESS;
}
