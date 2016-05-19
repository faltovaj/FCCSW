#ifndef SIMG4COMMON_MCTRUTHTRACKINFORMATION_H
#define SIMG4COMMON_MCTRUTHTRACKINFORMATION_H

// datamodel
//#include "datamodel/MCParticle.h"
//#include "datamodel/Particle.h"

// Geant4
#include "G4VUserTrackInformation.hh"

// CLHEP
//#include "CLHEP/Vector/ThreeVector.h"

/** @class MCTruthTrackInformation SimG4Common/SimG4Common/MCTruthTrackInformation.h MCTruthTrackInformation.h
 *
 *  Describes the information that can be assosiated with a G4Track, tracing MC truth particles
 *  
 *  @author Jana Faltova
 */

namespace sim {
class MCTruthTrackInformation: public G4VUserTrackInformation {
public:
  /** A constructor.
   *  @param[in] G4Track& aTrack
   */
  MCTruthTrackInformation();
  /// A destructor
  virtual ~MCTruthTrackInformation();
  /// A printing method
  virtual void Print() const final;
 
private:
  
};
}

#endif /* SIMG4COMMON_MCTRUTHUSERTRACKINFORMATION_H */
