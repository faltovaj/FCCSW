#include "G4SaveMCTruthParticles.h"

// FCCSW
#include "SimG4Common/Units.h"
#include "SimG4Common/MCTruthEventInformation.h"

// Geant4
#include "G4Event.hh"

// datamodel
#include "datamodel/MCParticleCollection.h"
#include "datamodel/GenVertexCollection.h"
#include "datamodel/MCParticle.h"
#include "datamodel/GenVertex.h"
#include "datamodel/GenVertexConst.h"

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
  fcc::MCParticleCollection* particleCollection = new fcc::MCParticleCollection();
  //  auto particles = m_particles.createAndPut();

  //create a collection of GenVertice
  fcc::GenVertexCollection* vertexCollection = new fcc::GenVertexCollection();
  //  auto vertices = m_vertices.createAndPut();

  //Retrieve the vector of MCParticles from event info and copy the information to the output collection
  auto vector_of_particles = mcevinf->GetVectorOfParticles();
  std::cout << "G4SaveMCTruthParticles:saveOutput, number of MCparticles to be stored: " << vector_of_particles.size() << std::endl;

  //Retrieve the vector of GenVertices from event info and copy the information to the output collection
  auto vector_of_vertices = mcevinf->GetVectorOfVertices();
  std::cout << "G4SaveMCTruthParticles:saveOutput, number of GenVertices to be stored: " << vector_of_vertices.size() << std::endl;

  for (auto iterator = vector_of_particles.begin(); iterator != vector_of_particles.end(); ++iterator) {
   
    //Create new particle
    fcc::MCParticle particle = particleCollection->create();
    particle.Core() = (*iterator)->Core();

    
   //Check if the vertex is already in the vector of vertices
    //if so, add the vertex as StartVertex or EndVertex and leave the loop
    //if not, add it to the vector if vertices
    bool findInitVertex = false;
    bool findEndVertex = false;
    
    for (auto iterator_vertex = vertexCollection->begin(); iterator_vertex != vertexCollection->end(); ++iterator_vertex)  {
      if ( SameVertex(*iterator_vertex, (*iterator)->StartVertex()) ) {
	findInitVertex = true;
	particle.StartVertex( *iterator_vertex ); 
	std::cout << "findInitVertex!!!!!!!!!!!" << std::endl;
      }
      
      if ( SameVertex(*iterator_vertex, (*iterator)->EndVertex()) ) {
	findInitVertex = true;
	particle.EndVertex( *iterator_vertex ); 
	std::cout << "findInitVertex!!!!!!!!!!!" << std::endl;
      } 
      if (findInitVertex && findEndVertex) break;
    }
    
    if (!findInitVertex) {
      //Add StartVertex
      fcc::GenVertex vertex = vertexCollection->create();
      vertex.Position() = (*iterator)->StartVertex().Position();
      vertex.Ctau( (*iterator)->StartVertex().Ctau() );
      particle.StartVertex( vertex );  
      std::cout << "StartVertex info "
              << " X " << vertex.Position().X
              << " Y " << vertex.Position().Y
              << " Z " << vertex.Position().Z <<  std::endl;
    }
    if (!findEndVertex) {
      //Add EndVertex
      fcc::GenVertex vertex = vertexCollection->create();
      vertex.Position() = (*iterator)->EndVertex().Position();
      vertex.Ctau( (*iterator)->EndVertex().Ctau() );
      particle.EndVertex( vertex );
      std::cout << "EndVertex info "
              << " X " << vertex.Position().X
              << " Y " << vertex.Position().Y
              << " Z " << vertex.Position().Z <<  std::endl;
    }
 
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

  }

  m_vertices.put(vertexCollection);
  m_particles.put(particleCollection);

  return StatusCode::SUCCESS;

}


bool G4SaveMCTruthParticles::SameVertex(const fcc::GenVertex& genVertex1, const fcc::ConstGenVertex& genVertex2) {
  if ( fabs(genVertex1.Position().X-genVertex2.Position().X)<epsilon_dist 
       && fabs(genVertex1.Position().Y-genVertex2.Position().Y)<epsilon_dist
       && fabs(genVertex1.Position().Z-genVertex2.Position().Z)<epsilon_dist  ) {
    return true;
  }
  return false;
}
