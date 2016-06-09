#include "SimG4Full/MCTruthActions.h"

//FCCSW
#include "SimG4Full/MCTruthSteppingAction.h"
#include "SimG4Full/MCTruthTrackingAction.h"
#include "SimG4Full/MCTruthEventAction.h"

namespace sim {
MCTruthActions::MCTruthActions():
  G4VUserActionInitialization() {}

MCTruthActions::~MCTruthActions() {}

void MCTruthActions::Build() const {
  SetUserAction(new MCTruthSteppingAction);
  SetUserAction(new MCTruthTrackingAction);
  SetUserAction(new MCTruthEventAction);
}
}
