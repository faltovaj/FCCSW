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

  //Retreive event info - vector of MCParticles and GenVertices to be stored
  sim::MCTruthEventInformation* mcevinf = (sim::MCTruthEventInformation*) aEvent.GetUserInformation();
  mcevinf->Print();

  //create a collection of MCParticles
  auto particles = m_particles.createAndPut();

  //Retrieve the vector of MCParticles from event info and copy the information to the output collection
  auto vector_of_particles = mcevinf->GetVectorOfParticles();
  std::cout << "G4SaveMCTruthParticles:saveOutput, number of MCparticles to be stored: " << vector_of_particles.size() << std::endl;

  for (auto iterator = vector_of_particles.begin(); iterator != vector_of_particles.end(); iterator++) {
   
    fcc::MCParticle particle = particles->create();
    particle.Core() = (*iterator)->Core();
    particle.StartVertex( (*iterator)->StartVertex() );
    particle.EndVertex( (*iterator)->EndVertex() );
 
    /*
    std::cout << "saved particle info " 
	      << " status " << (*iterator)->Core().Status
	      << " type " << (*iterator)->Core().Type
	      << " charge " << (*iterator)->Core().Charge 
	      << " px " << (*iterator)->Core().P4.Px  
	      << " py " << (*iterator)->Core().P4.Py
	      << " pz " << (*iterator)->Core().P4.Pz << std::endl;
    */
    std::cout << "MCparticle info "
              << " px " << particle.Core().P4.Px
              << " py " << particle.Core().P4.Py
              << " pz " << particle.Core().P4.Pz
      	      << " Start vertex x " << particle.StartVertex().Position().X 
	      << " y " << particle.StartVertex().Position().X
	      << " z " << particle.StartVertex().Position().Z 
	      << " End vertex x " << particle.EndVertex().Position().X
              << " y " << particle.EndVertex().Position().Y
              << " z " << particle.EndVertex().Position().Z 
	      << std::endl;

    /*
    fcc::GenVertex *vertex_test = new fcc::GenVertex();
    vertex_test->Position().X = 0.;
    vertex_test->Position().Y = 0.;
    vertex_test->Position().Z = 0.;
    vertex_test->Ctau(0.);    
    (*iterator)->StartVertex(*vertex_test);

    std::cout << "new collection test " <<  particle.StartVertex().Position().X << std::endl;
    */
   
  }

  //create a collection of GenVertices
  auto vertices = m_vertices.createAndPut();

  //Retrieve the vector of GenVertices from event info and copy the information to the output collection
  auto vector_of_vertices = mcevinf->GetVectorOfVertices();
  std::cout << "G4SaveMCTruthParticles:saveOutput, number of GenVertices to be stored: " << vector_of_vertices.size() << std::endl;

  for (auto iterator = vector_of_vertices.begin(); iterator != vector_of_vertices.end(); iterator++) {
    fcc::GenVertex vertex = vertices->create();
    vertex.Position() = (*iterator)->Position();
    vertex.Ctau( (*iterator)->Ctau() ); 

    std::cout << "GenVertex info "
              << " X " << vertex.Position().X
              << " Y " << vertex.Position().Y
              << " Z " << vertex.Position().Z <<  std::endl;

    /*
    for (auto& iterator_part=m_particles.begin(); iterator_part!=m_particles.end(); iterator_part++) { 
      if (iterator_part.StartVertex()==(*iterator_part)->StartVertex()) iterator_part.StartVertex(vertex);
      if (iterator_part.EndVertex()==(*iterator_part)->EndVertex()) iterator_part.EndVertex(vertex);
    }
    */

  }
    
  return StatusCode::SUCCESS;
}
