#ifndef SIMG4FULL_MCTRUTHTRACKINGACTION_H
#define SIMG4FULL_MCTRUTHTRACKINGACTION_H

//CLHEP
//#include "CLHEP/Vector/ThreeVector.h"

// Geant
#include "G4UserTrackingAction.hh"
#include "G4ThreeVector.hh"

/**
 *
 *  Tracking action (invoked before/after Geant track processing).
 * 
 *
 *  @author Jana Faltova
*/

namespace sim {
class MCTruthTrackingAction : public G4UserTrackingAction {
public:
  MCTruthTrackingAction();
  virtual ~MCTruthTrackingAction();
  /* Defines the actions at the beginning or end of processing the track.
   * It save the information about momentum, status and vertex in the
   * information associated with MC Truth Particles
   */
  virtual void PreUserTrackingAction(const G4Track*) final;
  virtual void PostUserTrackingAction(const G4Track*) final;
private:
  G4ThreeVector aInitMom;
  G4ThreeVector prodPosition;
  G4ThreeVector endPosition;
  bool trackToBeStored(const G4Track* aTrack);
};
}

#endif /* SIMG4FULL_MCTRUTHTRACKINGACTION_H */


