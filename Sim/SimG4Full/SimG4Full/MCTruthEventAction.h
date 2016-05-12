#ifndef SIMG4FULL_MCTRUTHEVENTACTION_H
#define SIMG4FULL_MCTRUTHEVENTACTION_H

// Geant
#include "G4UserEventAction.hh"

/**
 *
 *  Event action
 * 
 *
 *  @author Jana Faltova
*/

namespace sim {
class MCTruthEventAction : public G4UserEventAction {

   public:
        MCTruthEventAction();
        virtual ~MCTruthEventAction();
        virtual void BeginOfEventAction(const G4Event*);
        virtual void EndOfEventAction(const G4Event*);
      
};
}

#endif /* SIMG4FULL_MCTRUTHEVENTACTION_H */


