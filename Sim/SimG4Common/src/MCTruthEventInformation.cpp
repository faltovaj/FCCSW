#include "SimG4Common/MCTruthEventInformation.h"

// FCCSW
#include "SimG4Common/Units.h"

// datamodel                                                                                                                                         
#include "datamodel/MCParticle.h"
#include "datamodel/GenVertex.h"
#include "datamodel/MCParticleCollection.h"
#include "datamodel/GenVertexCollection.h"

namespace sim {
MCTruthEventInformation::MCTruthEventInformation() {
  //  m_vector_mcparticle.clear();
  //  m_vector_genvertex.clear();
}

MCTruthEventInformation::~MCTruthEventInformation() {}

void MCTruthEventInformation::Print() const {
  
  std::cout << "MCTruthEventInformation: Number of MC truth particles stored: " <<  m_vector_mcparticle.size() << std::endl;
  /*
  for (unsigned int i = 0; i<m_vector_mcparticle.size(); i++) {
    std::cout << "stored particle info " 
	    << " px " << m_vector_mcparticle.at(i)->Core().P4.Px  
	    << " py " << m_vector_mcparticle.at(i)->Core().P4.Py
	    << " pz " << m_vector_mcparticle.at(i)->Core().P4.Pz << std::endl;
  }
  */
}

void MCTruthEventInformation::AddParticle(const G4ThreeVector& aMom, const G4ThreeVector& aInitVertex, const G4ThreeVector& aEndVertex) {

    fcc::MCParticle* edmMCparticle = new fcc::MCParticle();
    fcc::BareParticle& core = edmMCparticle->Core();
    core.P4.Px = aMom.x()*sim::g42edm::energy;
    core.P4.Py = aMom.y()*sim::g42edm::energy;
    core.P4.Pz = aMom.z()*sim::g42edm::energy;
    //core.P4.Mass = aMom.m()*sim::g42edm::energy;
    core.Vertex.X = aInitVertex.x()*sim::g42edm::length;
    core.Vertex.Y = aInitVertex.y()*sim::g42edm::length;
    core.Vertex.Z = aInitVertex.z()*sim::g42edm::length;

    m_vector_mcparticle.push_back(edmMCparticle);
    //m_collection_mcparticle.push_back(edmMCparticle);

}

const std::vector<fcc::MCParticle*> MCTruthEventInformation::GetVectorOfParticles() {
    return m_vector_mcparticle;
}

const std::vector<fcc::GenVertex*> MCTruthEventInformation::GetVectorOfGenVertices() {
    return m_vector_genvertex;
}

  /*
const fcc::MCParticleCollection MCTruthEventInformation::GetCollectionOfParticles() {
    return m_collection_mcparticle;
}
  */
}
