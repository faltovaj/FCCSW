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
      if ( SameVertex_4vector(aInitVertex, (*iterator)->Position()) ) {
	findInitVertex = true;
	edmMCparticle->StartVertex( **iterator ); 
	//std::cout << "findInitVertex!!!!!!!!!!!" << std::endl;
      }
      if ( SameVertex_4vector(aEndVertex, (*iterator)->Position()) ) {        
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

  void MCTruthEventInformation::AddVertex(G4Track* aTrack, G4StepPoint* postStepPoint, G4TrackVector& secondaries) {

    fcc::MCParticle* edmMCparticle = new fcc::MCParticle();
    fcc::BareParticle& core = edmMCparticle->Core();
    core.P4.Px = postStepPoint->GetMomentum().x()*sim::g42edm::energy;
    core.P4.Py = postStepPoint->GetMomentum().y()*sim::g42edm::energy;
    core.P4.Pz = postStepPoint->GetMomentum().z()*sim::g42edm::energy;
    core.P4.Mass = aTrack->GetDynamicParticle()->GetDefinition()->GetPDGMass()*sim::g42edm::energy;
    core.Vertex.X = aTrack->GetVertexPosition().x()*sim::g42edm::length;
    core.Vertex.Y = aTrack->GetVertexPosition().y()*sim::g42edm::length;
    core.Vertex.Z = aTrack->GetVertexPosition().z()*sim::g42edm::length;
    core.Charge = postStepPoint->GetCharge();
    core.Type = aTrack->GetDynamicParticle()->GetPDGcode();
    core.Status = 10e6;

    //Check if the vertex is already in the vector of vertices
    //if so, add the vertex as StartVertex or EndVertex and leave the loop
    //if not, add it to the vector if vertices
    CheckAndAddVertex( aTrack->GetVertexPosition(), postStepPoint->GetPosition(), edmMCparticle);

    m_vector_mcparticle.push_back(edmMCparticle);
    //m_collection_mcparticle.push_back(edmMCparticle); 

    //Store secondaries
    for (auto iterator = secondaries.begin(); iterator!=secondaries.end(); iterator++) {

      fcc::MCParticle* edmMCparticle = new fcc::MCParticle();
      fcc::BareParticle& core = edmMCparticle->Core();
      core.P4.Px =  (*iterator)->GetMomentum().x()*sim::g42edm::energy;
      core.P4.Py =  (*iterator)->GetMomentum().y()*sim::g42edm::energy;
      core.P4.Pz =  (*iterator)->GetMomentum().z()*sim::g42edm::energy;
      core.P4.Mass = (*iterator)->GetDynamicParticle()->GetDefinition()->GetPDGMass()*sim::g42edm::energy;
      core.Vertex.X = (*iterator)->GetPosition().x()*sim::g42edm::length;
      core.Vertex.Y = (*iterator)->GetPosition().y()*sim::g42edm::length;
      core.Vertex.Z = (*iterator)->GetPosition().z()*sim::g42edm::length;
      core.Charge = (*iterator)->GetDynamicParticle()->GetCharge();
      core.Type = (*iterator)->GetDynamicParticle()->GetPDGcode();
      core.Status = 10e6;
      
      //Check if the vertex is already in the vector of vertices
      //if so, add the vertex as StartVertex or EndVertex and leave the loop
      //if not, add it to the vector if vertices
      G4ThreeVector EmptyEndPosition(10e6,10e6,10e6);
      CheckAndAddVertex( (*iterator)->GetPosition(), EmptyEndPosition, edmMCparticle);

      m_vector_mcparticle.push_back(edmMCparticle);

    }


  }


void MCTruthEventInformation::CheckAndAddVertex(G4ThreeVector g4threeVector_in, G4ThreeVector g4threeVector_out, fcc::MCParticle* edmMCparticle)
  {
    bool findInitVertex = false;
    bool findEndVertex = false;
    for (auto iterator = m_vector_genvertex.begin(); iterator != m_vector_genvertex.end(); iterator++)  {
      if ( SameVertex_3vector(g4threeVector_in, (*iterator)->Position()) ) {
	findInitVertex = true;
	edmMCparticle->StartVertex( **iterator ); 
	//std::cout << "findInitVertex!!!!!!!!!!!" << std::endl;
      }
      if ( SameVertex_3vector(g4threeVector_out, (*iterator)->Position()) ) {        
	findEndVertex = true;
	edmMCparticle->EndVertex( **iterator );
	//std::cout <<"findEndVertex!!!!!!!!!!!" << std::endl;
      }
      if (findInitVertex && findEndVertex) break;
    }

    if (!findInitVertex) {
      fcc::GenVertex* edmGenVertex = new fcc::GenVertex();
      edmGenVertex->Position().X = g4threeVector_in.x()*sim::g42edm::length;
      edmGenVertex->Position().Y = g4threeVector_in.y()*sim::g42edm::length;
      edmGenVertex->Position().Z = g4threeVector_in.z()*sim::g42edm::length;
      edmGenVertex->Ctau( 0.0 );
      m_vector_genvertex.push_back(edmGenVertex);
      edmMCparticle->StartVertex( *edmGenVertex );
      //std::cout << "time " << aInitVertex.t() << std::endl;  
    }
    if (!findEndVertex) {
      fcc::GenVertex* edmGenVertex = new fcc::GenVertex();
      edmGenVertex->Position().X = g4threeVector_out.x();
      edmGenVertex->Position().Y = g4threeVector_out.y();
      edmGenVertex->Position().Z = g4threeVector_out.z();
      edmGenVertex->Ctau( 0.0 );
      m_vector_genvertex.push_back(edmGenVertex);
      edmMCparticle->EndVertex( *edmGenVertex );    
    }
 }


const std::vector<fcc::MCParticle*> MCTruthEventInformation::GetVectorOfParticles() {
    return m_vector_mcparticle;
}

const std::vector<fcc::GenVertex*> MCTruthEventInformation::GetVectorOfVertices() {
    return m_vector_genvertex;
}

bool MCTruthEventInformation::SameVertex_4vector(const G4LorentzVector& g4lorentzVertex, fcc::Point& fccPoint) {
  //std::cout << "In SameVertex function: distance " << sqrt(pow(g4lorentzVertex.x()*sim::g42edm::length-fccPoint.X,2)+pow(g4lorentzVertex.y()*sim::g42edm::length-fccPoint.Y,2)+pow(g4lorentzVertex.z()*sim::g42edm::length-fccPoint.Z,2)) << std::endl;
  if ( fabs(g4lorentzVertex.x()*sim::g42edm::length-fccPoint.X)<epsilon_dist 
       && fabs(g4lorentzVertex.y()*sim::g42edm::length-fccPoint.Y)<epsilon_dist
       && fabs(g4lorentzVertex.z()*sim::g42edm::length-fccPoint.Z)<epsilon_dist  ) {
    return true;
  }
  return false;
}

bool MCTruthEventInformation::SameVertex_3vector(const G4ThreeVector& g4threeVertex, fcc::Point& fccPoint) {
  //std::cout << "In SameVertex function: distance " << sqrt(pow(g4lorentzVertex.x()*sim::g42edm::length-fccPoint.X,2)+pow(g4lorentzVertex.y()*sim::g42edm::length-fccPoint.Y,2)+pow(g4lorentzVertex.z()*sim::g42edm::length-fccPoint.Z,2)) << std::endl;
  if ( fabs(g4threeVertex.x()*sim::g42edm::length-fccPoint.X)<epsilon_dist 
       && fabs(g4threeVertex.y()*sim::g42edm::length-fccPoint.Y)<epsilon_dist
       && fabs(g4threeVertex.z()*sim::g42edm::length-fccPoint.Z)<epsilon_dist  ) {
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
