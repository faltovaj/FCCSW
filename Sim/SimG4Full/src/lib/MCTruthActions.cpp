#include "SimG4Full/MCTruthActions.h"

//FCCSW
#include "SimG4Full/MCTruthTrackingAction.h"

namespace sim {
MCTruthActions::MCTruthActions():
  G4VUserActionInitialization() {}

MCTruthActions::~MCTruthActions() {}

void MCTruthActions::Build() const {
  SetUserAction(new MCTruthTrackingAction);
}
}
