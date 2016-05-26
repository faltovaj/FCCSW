#include "SimG4Common/MCTruthEventInformation.h"

// FCCSW
#include "SimG4Common/Units.h"

// datamodel                                                                                                                                         
#include "datamodel/MCParticle.h"
#include "datamodel/GenVertex.h"
//#include "datamodel/MCParticleCollection.h"
//#include "datamodel/GenVertexCollection.h"

namespace sim {
MCTruthEventInformation::MCTruthEventInformation() {
}

MCTruthEventInformation::~MCTruthEventInformation() {}

void MCTruthEventInformation::Print() const {
  
  std::cout << "MCTruthEventInformation: Number of MC truth particles stored: " <<  m_vector_mcparticle.size() << std::endl;
  std::cout << "MCTruthEventInformation: Number of MC gen vertices stored: " <<  m_vector_genvertex.size() << std::endl;
  /*
  for (unsigned int i = 0; i<m_vector_mcparticle.size(); i++) {
    std::cout << "stored particle info " 
	    << " px " << m_vector_mcparticle.at(i)->Core().P4.Px  
	    << " py " << m_vector_mcparticle.at(i)->Core().P4.Py
	    << " pz " << m_vector_mcparticle.at(i)->Core().P4.Pz << std::endl;
  }
  */
}

void MCTruthEventInformation::AddParticle(const G4LorentzVector& aMom, const G4LorentzVector& aInitVertex, const G4LorentzVector& aEndVertex, G4int aPDGcode, G4int aCharge, G4int aStatus) {

    fcc::MCParticle* edmMCparticle = new fcc::MCParticle();
    fcc::BareParticle& core = edmMCparticle->Core();
    core.P4.Px = aMom.px()*sim::g42edm::energy;
    core.P4.Py = aMom.py()*sim::g42edm::energy;
    core.P4.Pz = aMom.pz()*sim::g42edm::energy;
    core.P4.Mass = aMom.m()*sim::g42edm::energy;
    core.Vertex.X = aInitVertex.x()*sim::g42edm::length;
    core.Vertex.Y = aInitVertex.y()*sim::g42edm::length;
    core.Vertex.Z = aInitVertex.z()*sim::g42edm::length;
    core.Charge = aCharge;
    core.Type = aPDGcode;
    core.Status = aStatus;

    //Check if the vertex is already in the vector of vertices
    //if so, add the vertex as StartVertex or EndVertex and leave the loop
    //if not, add it to the vector if vertices
    bool findInitVertex = false;
    bool findEndVertex = false;
    for (auto iterator = m_vector_genvertex.begin(); iterator != m_vector_genvertex.end(); iterator++)  {
      if ( SameVertex(aInitVertex, (*iterator)->Position()) ) {
	findInitVertex = true;
	edmMCparticle->StartVertex( **iterator ); 
	//std::cout << "findInitVertex!!!!!!!!!!!" << std::endl;
      }
      if ( SameVertex(aEndVertex, (*iterator)->Position()) ) {        
	findEndVertex = true;
	edmMCparticle->EndVertex( **iterator );
	//std::cout <<"findEndVertex!!!!!!!!!!!" << std::endl;
      }
      if (findInitVertex && findEndVertex) break;
    }

    if (!findInitVertex) {
      fcc::GenVertex* edmGenVertex = new fcc::GenVertex();
      edmGenVertex->Position().X = aInitVertex.x()*sim::g42edm::length;
      edmGenVertex->Position().Y= aInitVertex.y()*sim::g42edm::length;
      edmGenVertex->Position().Z= aInitVertex.z()*sim::g42edm::length;
      edmGenVertex->Ctau( aInitVertex.t()*sim::g42edm::length );
      m_vector_genvertex.push_back(edmGenVertex);
      edmMCparticle->StartVertex( *edmGenVertex );
      //std::cout << "time " << aInitVertex.t() << std::endl;  
    }
    if (!findEndVertex) {
      fcc::GenVertex* edmGenVertex = new fcc::GenVertex();
      edmGenVertex->Position().X= aEndVertex.x();
      edmGenVertex->Position().Y= aEndVertex.y();
      edmGenVertex->Position().Z= aEndVertex.z();
      m_vector_genvertex.push_back(edmGenVertex);
      edmMCparticle->EndVertex( *edmGenVertex );    
    }

    m_vector_mcparticle.push_back(edmMCparticle);
    //m_collection_mcparticle.push_back(edmMCparticle);        

}

const std::vector<fcc::MCParticle*> MCTruthEventInformation::GetVectorOfParticles() {
    return m_vector_mcparticle;
}

const std::vector<fcc::GenVertex*> MCTruthEventInformation::GetVectorOfVertices() {
    return m_vector_genvertex;
}

bool MCTruthEventInformation::SameVertex(const G4LorentzVector& g4lorentzVertex, fcc::Point& fccPoint) {
  //std::cout << "In SameVertex function: distance " << sqrt(pow(g4lorentzVertex.x()*sim::g42edm::length-fccPoint.X,2)+pow(g4lorentzVertex.y()*sim::g42edm::length-fccPoint.Y,2)+pow(g4lorentzVertex.z()*sim::g42edm::length-fccPoint.Z,2)) << std::endl;
  if ( fabs(g4lorentzVertex.x()*sim::g42edm::length-fccPoint.X)<epsilon_dist 
       && fabs(g4lorentzVertex.y()*sim::g42edm::length-fccPoint.Y)<epsilon_dist
       && fabs(g4lorentzVertex.z()*sim::g42edm::length-fccPoint.Z)<epsilon_dist  ) {
    return true;
  }
  return false;
}

  /*
const fcc::MCParticleCollection MCTruthEventInformation::GetCollectionOfParticles() {
    return m_collection_mcparticle;
}
  */
}
