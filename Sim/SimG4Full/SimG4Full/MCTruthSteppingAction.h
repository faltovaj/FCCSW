#ifndef SIMG4FULL_MCTRUTHSTEPPINGACTION_H
#define SIMG4FULL_MCTRUTHSTEPPINGACTION_H

//CLHEP
//#include "CLHEP/Vector/ThreeVector.h"

// Geant
#include "G4UserSteppingAction.hh"
#include "G4ThreeVector.hh"
#include "G4LorentzVector.hh"
#include "G4TrackStatus.hh"
#include "G4Track.hh"
#include "G4TrackVector.hh"


/**
 *
 *  Stepping action (invoked before/after Geant track processing).
 * 
 *
 *  @author Jana Faltova
*/

namespace sim {
class MCTruthSteppingAction : public G4UserSteppingAction {
public:
  MCTruthSteppingAction();
  virtual ~MCTruthSteppingAction();
  /* Defines the actions at the beginning or end of processing the track.
   * It save the information about momentum, status and vertex in the
   * information associated with MC Truth Particles
   */
  virtual void UserSteppingAction(const G4Step*) final;
private:
  
  G4TrackVector secondaries_toBeStored;

  bool bremsstrahlung_processToBeStored(const G4Step* aStep);
};
}

#endif /* SIMG4FULL_MCTRUTHSTEPPINGACTION_H */


