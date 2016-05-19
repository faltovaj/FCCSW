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

  mcevinf->Print();

  auto vector_of_particles = mcevinf->GetVectorOfParticles();

  std::cout << "G4SaveMCTruthParticles:saveOutput, number of MCparticles to be stored: " << vector_of_particles.size() << std::endl;

  //m_particles.put(vector_of_particles);

  for (unsigned int i = 0; i<vector_of_particles.size(); i++) {
   
    fcc::MCParticle particle = particles->create();
    //particle = *vector_of_particles.at(i);

    fcc::BareParticle& particle_core = particle.Core();
    particle_core.P4.Px = vector_of_particles.at(i)->Core().P4.Px;
    particle_core.P4.Py = vector_of_particles.at(i)->Core().P4.Py;
    particle_core.P4.Pz = vector_of_particles.at(i)->Core().P4.Pz;
    particle_core.P4.Mass = vector_of_particles.at(i)->Core().P4.Mass;
    particle_core.Vertex.X = vector_of_particles.at(i)->Core().Vertex.X;
    particle_core.Vertex.Y = vector_of_particles.at(i)->Core().Vertex.Y;
    particle_core.Vertex.Z = vector_of_particles.at(i)->Core().Vertex.Z;
    particle_core.Charge = vector_of_particles.at(i)->Core().Charge;
    particle_core.Type = vector_of_particles.at(i)->Core().Type;
    particle_core.Status = vector_of_particles.at(i)->Core().Status;
    particle_core.Bits = vector_of_particles.at(i)->Core().Bits;

    std::cout << "saved particle info " 
	      << " status " << vector_of_particles.at(i)->Core().Status
	      << " type " << vector_of_particles.at(i)->Core().Type
	      << " charge " << vector_of_particles.at(i)->Core().Charge 
	      << " px " << vector_of_particles.at(i)->Core().P4.Px  
	      << " py " << vector_of_particles.at(i)->Core().P4.Py
	      << " pz " << vector_of_particles.at(i)->Core().P4.Pz << std::endl;
    std::cout << "new collection particle info "
              << " px " << particles->at(i).Core().P4.Px
              << " py " << particles->at(i).Core().P4.Py
              << " pz " << particles->at(i).Core().P4.Pz << std::endl;




    /*   
    std::cout << "G4SaveMCTruthParticles: index " << i 
	      << " px " << particle.Core().P4.Px
	      << " py " << particle.Core().P4.Py 
	      << " pz " << particle.Core().P4.Pz << std::endl;
    */
   }
    

  return StatusCode::SUCCESS;
}
