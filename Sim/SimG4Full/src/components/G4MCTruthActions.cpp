#include "G4MCTruthActions.h"

// FCCSW
#include "SimG4Full/MCTruthActions.h"

DECLARE_COMPONENT(G4MCTruthActions)

G4MCTruthActions::G4MCTruthActions(const std::string& type, const std::string& name, const IInterface* parent) :
  AlgTool(type, name, parent) {
   declareInterface<IG4ActionTool>(this);
}

G4MCTruthActions::~G4MCTruthActions() {}

StatusCode G4MCTruthActions::initialize() {
   if(AlgTool::initialize().isFailure()) {
      return StatusCode::FAILURE;
   }
   return StatusCode::SUCCESS;
}

StatusCode G4MCTruthActions::finalize() {
  return AlgTool::finalize();
}

G4VUserActionInitialization* G4MCTruthActions::getUserActionInitialization() {
  return new sim::MCTruthActions;
}
