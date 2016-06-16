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

void MCTruthEventInformation::AddParticle(const G4Track* aTrack, const G4LorentzVector& aMom, const G4int aStatus) {

  fcc::MCParticle* edmMCparticle = new fcc::MCParticle();
  fcc::BareParticle& core = edmMCparticle->Core();

  trackToParticle(aTrack, edmMCparticle, true);

  //Momentum at the beginning of the track
  core.P4.Px = aMom.px()*sim::g42edm::energy;
  core.P4.Py = aMom.py()*sim::g42edm::energy;
  core.P4.Pz = aMom.pz()*sim::g42edm::energy;
  core.Status = aStatus;
    
  //Check if the vertex is already in the vector of vertices
  //if so, add the vertex as StartVertex or EndVertex and leave the loop
  //if not, add it to the vector if vertices
  //CheckAndAddVertex( aInitVertex, aEndVertex, edmMCparticle);
  
  m_vector_mcparticle.push_back(edmMCparticle);
  //m_collection_mcparticle.push_back(edmMCparticle);        

}

  void MCTruthEventInformation::AddVertex(const G4Track* aTrack, const G4StepPoint* postStepPoint, const G4TrackVector& secondaries) {

    fcc::MCParticle* edmMCparticle_out = new fcc::MCParticle();
    fcc::BareParticle& core_out = edmMCparticle_out->Core();
    core_out.P4.Px = postStepPoint->GetMomentum().x()*sim::g42edm::energy;
    core_out.P4.Py = postStepPoint->GetMomentum().y()*sim::g42edm::energy;
    core_out.P4.Pz = postStepPoint->GetMomentum().z()*sim::g42edm::energy;

    bool is_incoming = false;
    //Add information about MCParticle and GenVertex from G4Track information
    trackToParticle(aTrack, edmMCparticle_out, is_incoming);
    //set Status to 10 for bremstralung
    core_out.Status = 10;
    m_vector_mcparticle.push_back(edmMCparticle_out);
    
    
    fcc::MCParticle* edmMCparticle_in = new fcc::MCParticle();
    fcc::BareParticle& core_in = edmMCparticle_in->Core();
    is_incoming = true;
    trackToParticle(aTrack, edmMCparticle_in, is_incoming);
    //set Status to 10 for bremstralung
    core_in.Status = 11;
    //std::cout << "== Control: endvertex " << aEndVertex.x() << " postStep " <<  postStepPoint->GetPosition().x() << std::endl;
    m_vector_mcparticle.push_back(edmMCparticle_in);

    //Store secondaries
    for (auto iterator = secondaries.begin(); iterator!=secondaries.end(); iterator++) {

      fcc::MCParticle* edmMCparticle = new fcc::MCParticle();
      fcc::BareParticle& core = edmMCparticle->Core();
      core.P4.Px =  (*iterator)->GetMomentum().x()*sim::g42edm::energy;
      core.P4.Py =  (*iterator)->GetMomentum().y()*sim::g42edm::energy;
      core.P4.Pz =  (*iterator)->GetMomentum().z()*sim::g42edm::energy;
      is_incoming = false;
      trackToParticle(*iterator, edmMCparticle, is_incoming);
      //set Status to 20 for bremstralung secondaries
      core.Status = 20;
      m_vector_mcparticle.push_back(edmMCparticle);

    }

  }


void MCTruthEventInformation::CheckAndAddVertex(const G4ThreeVector g4threeVector_in, const G4ThreeVector g4threeVector_out, fcc::MCParticle* edmMCparticle)
  {
    //std::cout << "CheckAndAddVertex" << std::endl;
    bool findInitVertex = false;
    bool findEndVertex = false;
    for (auto iterator = m_vector_genvertex.begin(); iterator != m_vector_genvertex.end(); iterator++)  {
      if ( SameVertex(g4threeVector_in, (*iterator)->Position()) ) {
	findInitVertex = true;
	edmMCparticle->StartVertex( **iterator ); 
	//std::cout << "findInitVertex!!!!!!!!!!!" << std::endl;
      }
      if ( SameVertex(g4threeVector_out, (*iterator)->Position()) ) {        
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

  void MCTruthEventInformation::trackToParticle(const G4Track* aTrack, fcc::MCParticle* edmMCparticle, bool is_incoming) {

   fcc::BareParticle& core = edmMCparticle->Core();

    core.P4.Mass = aTrack->GetDynamicParticle()->GetDefinition()->GetPDGMass()*sim::g42edm::energy;

    core.Charge = aTrack->GetDynamicParticle()->GetCharge();
    core.Type = aTrack->GetDynamicParticle()->GetPDGcode();
    //trackID in Bits
    core.Bits = aTrack->GetTrackID();

    std::cout << "trackToParticle pdg " << aTrack->GetDynamicParticle()->GetPDGcode() << " trackID " << aTrack->GetTrackID() << " mother ID " << aTrack->GetParentID() << std::endl;

    G4ThreeVector aInitVertex;
    G4ThreeVector aEndVertex;

    if (is_incoming) {
      double momentum_amplitude = sqrt(pow(aTrack->GetVertexKineticEnergy(),2)+2*aTrack->GetVertexKineticEnergy()*aTrack->GetDynamicParticle()->GetDefinition()->GetPDGMass()) ;
      
      //momentum_amplitude = 1.0;
      core.P4.Px = momentum_amplitude*aTrack->GetVertexMomentumDirection().x()*sim::g42edm::energy;
      core.P4.Py = momentum_amplitude*aTrack->GetVertexMomentumDirection().y()*sim::g42edm::energy;
      core.P4.Pz = momentum_amplitude*aTrack->GetVertexMomentumDirection().z()*sim::g42edm::energy;
      
      core.Vertex.X = aTrack->GetVertexPosition().x()*sim::g42edm::length;
      core.Vertex.Y = aTrack->GetVertexPosition().y()*sim::g42edm::length;
      core.Vertex.Z = aTrack->GetVertexPosition().z()*sim::g42edm::length;
      
      aInitVertex = aTrack->GetVertexPosition()*sim::g42edm::length;
      aEndVertex = aTrack->GetPosition()*sim::g42edm::length;
      //std::cout << " init x " << aTrack->GetVertexPosition().x() << " end x " << aTrack->GetPosition().x() << std::endl;
    }
    else {
      core.Vertex.X = aTrack->GetPosition().x()*sim::g42edm::length;
      core.Vertex.Y = aTrack->GetPosition().y()*sim::g42edm::length;
      core.Vertex.Z = aTrack->GetPosition().z()*sim::g42edm::length;

      aInitVertex = aTrack->GetPosition()*sim::g42edm::length;
      G4ThreeVector EmptyEndPosition(1e6,1e6,1e6);
      //aEndVertex = aTrack->GetPosition()*sim::g42edm::length;
      aEndVertex = EmptyEndPosition;
    }

    //Check if the vertex is already in the vector of vertices
    //if so, add the vertex as StartVertex or EndVertex and leave the loop
    //if not, add it to the vector if vertices
    CheckAndAddVertex(aInitVertex, aEndVertex, edmMCparticle);

  }



const std::vector<fcc::MCParticle*> MCTruthEventInformation::GetVectorOfParticles() {
    return m_vector_mcparticle;
}

const std::vector<fcc::GenVertex*> MCTruthEventInformation::GetVectorOfVertices() {
    return m_vector_genvertex;
}


bool MCTruthEventInformation::SameVertex(const G4ThreeVector& g4threeVertex, const fcc::Point& fccPoint) {
  //std::cout << "In SameVertex function: distance " << sqrt(pow(g4lorentzVertex.x()*sim::g42edm::length-fccPoint.X,2)+pow(g4lorentzVertex.y()*sim::g42edm::length-fccPoint.Y,2)+pow(g4lorentzVertex.z()*sim::g42edm::length-fccPoint.Z,2)) << std::endl;
  if ( fabs(g4threeVertex.x()-fccPoint.X)<epsilon_dist 
       && fabs(g4threeVertex.y()-fccPoint.Y)<epsilon_dist
       && fabs(g4threeVertex.z()-fccPoint.Z)<epsilon_dist  ) {
    return true;
  }
  return false;
}

}
