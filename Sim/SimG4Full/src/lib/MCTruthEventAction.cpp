#include "SimG4Full/MCTruthEventAction.h"

//FCCSW
#include "SimG4Common/Units.h"
#include "SimG4Common/MCTruthEventInformation.h"

// datamodel
//#include "datamodel/MCParticle.h"

// Geant4
#include "G4EventManager.hh"
#include "G4ThreeVector.hh"
#include "G4VUserEventInformation.hh"

namespace sim {
MCTruthEventAction::MCTruthEventAction():
  G4UserEventAction() {}
MCTruthEventAction::~MCTruthEventAction() {}

void MCTruthEventAction::BeginOfEventAction(const G4Event* aEvent) {

  
  if(!aEvent->GetUserInformation()) 
    {
      MCTruthEventInformation* mcinf = new MCTruthEventInformation;
      (G4EventManager::GetEventManager())->SetUserInformation(mcinf);
    }
  
  return;
}

void MCTruthEventAction::EndOfEventAction(const G4Event* aEvent) {
    
  std::cout << " EndOfEventAction" << std::endl; 
  return;
}

}
