#ifndef SIMG4FULL_MCTRUTHACTIONS_H
#define SIMG4FULL_MCTRUTHACTIONS_H

#include "G4VUserActionInitialization.hh"

/** @class MCTruthActions SimG4Full/SimG4Full/MCTruthActions.h MCTruthActions.h
 *
 *  User action initialization for full simulation.
 *
 *  @author Jana Faltova
 */

namespace sim {
class MCTruthActions : public G4VUserActionInitialization {
public:
   MCTruthActions();
   ~MCTruthActions();
   /// Create all user actions.
   virtual void Build() const final;
};
}

#endif /* SIMG4FULL_MCTRUTHACTIONS_H */
