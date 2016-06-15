
#ifndef SIMG4COMMON_MCTRUTHEVENTINFORMATION_H
#define SIMG4COMMON_MCTRUTHEVENTINFORMATION_H

// datamodel
#include "datamodel/MCParticle.h"
#include "datamodel/GenVertex.h"
#include "datamodel/Point.h"
#include "datamodel/MCParticleCollection.h"
#include "datamodel/GenVertexCollection.h"

// Geant4
#include "G4VUserEventInformation.hh"
#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include "G4LorentzVector.hh"
#include "G4StepPoint.hh"

// CLHEP
//#include "CLHEP/Vector/ThreeVector.h"

/** @class MCTruthEventInformation SimG4Common/SimG4Common/MCTruthEventInformation.h MCTruthEventInformation.h
 *
 *  Describes the information that can be assosiated with a G4Track, tracing MC truth particles
 *  
 *  @author Jana Faltova
 */

namespace sim {
class MCTruthEventInformation: public G4VUserEventInformation {
public:
  /** A constructor.
   *  @param[in] G4Event& aEvent
   */
  MCTruthEventInformation();
  /// A destructor
  virtual ~MCTruthEventInformation();
  /// A printing method
  virtual void Print() const final;
 
  /** 
   *  
   */
  void AddParticle(const G4Track *aTrack, const G4LorentzVector& aMom, const G4int aStatus);

  void AddVertex(const G4Track* aTrack, const G4StepPoint* postStepPoint, const G4TrackVector& secondaries_toBeStored); 

  void trackToParticle(const G4Track* aTrack, fcc::MCParticle* edmMCparticle, bool is_incoming);

  const std::vector<fcc::MCParticle*> GetVectorOfParticles();

  const std::vector<fcc::GenVertex*> GetVectorOfVertices(); 


private:
  void CheckAndAddVertex(const G4ThreeVector g4threeVector_in, const G4ThreeVector g4threeVector_out, fcc::MCParticle* edmMCparticle);
  bool SameVertex(const G4ThreeVector& g4threeVertex, const fcc::Point& fccPoint);

  /// EDM MC particle
  std::vector<fcc::MCParticle*> m_vector_mcparticle;
  std::vector<fcc::GenVertex*> m_vector_genvertex;
  float epsilon_dist = 1e-4;
  //fcc::MCParticleCollection m_collection_mcparticle;  

};
}

#endif /* SIMG4COMMON_MCTRUTHEVENTTRACKINFORMATION_H */
