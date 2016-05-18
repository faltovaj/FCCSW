#ifndef SIMG4COMMON_MCTRUTHEVENTINFORMATION_H
#define SIMG4COMMON_MCTRUTHEVENTINFORMATION_H

// datamodel
//#include "datamodel/MCParticle.h"
//#include "datamodel/Particle.h"

// Geant4
#include "G4VUserEventInformation.hh"

// CLHEP
#include "CLHEP/Vector/ThreeVector.h"

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
  void AddParticle(const CLHEP::Hep3Vector& aMom);
 
private:
  /// EDM MC particle
  CLHEP::Hep3Vector m_initMomentum;
  
};
}

#endif /* SIMG4COMMON_MCTRUTHEVENTTRACKINFORMATION_H */
