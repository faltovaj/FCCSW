#include "SimG4Full/MCTruthSteppingAction.h"

//FCCSW
#include "SimG4Common/Units.h"
#include "SimG4Common/MCTruthTrackInformation.h"
#include "SimG4Common/MCTruthEventInformation.h"

// Geant4
#include "G4Event.hh"
#include "G4Track.hh"
#include "G4EventManager.hh"
#include "G4TrackingManager.hh"
#include "G4PrimaryParticle.hh"
#include "G4ThreeVector.hh"
#include "G4TrackVector.hh"
#include "G4VUserTrackInformation.hh"
#include "G4StepPoint.hh"

namespace sim {
MCTruthSteppingAction::MCTruthSteppingAction():
  G4UserSteppingAction() {
}

MCTruthSteppingAction::~MCTruthSteppingAction() {}

void MCTruthSteppingAction::UserSteppingAction(const G4Step* aStep) {
 
  if ( bremstralung_processToBeStored(aStep) ) {
   
   G4Track* aTrack = aStep->GetTrack();
   G4StepPoint* postStepPoint = aStep->GetPostStepPoint();

    MCTruthTrackInformation* mcinf = (MCTruthTrackInformation*) aTrack->GetUserInformation();
    const G4Event *aEvent = (G4EventManager::GetEventManager())->GetConstCurrentEvent();
    MCTruthEventInformation* mcevinf = (MCTruthEventInformation*) aEvent->GetUserInformation();
   
    mcevinf->AddVertex(aTrack, postStepPoint, secondaries_toBeStored);
   
  }
  else
  {
  
  }


  return;
}

bool MCTruthSteppingAction::bremstralung_processToBeStored(const G4Step* aStep)
{

  bool pass = true;  

  double minE = 10.;
  double minE_secondaries = 1.;

  //check energy(momentum)
  double momentum = aStep->GetPostStepPoint()->GetMomentum().mag();
  
  if (momentum < minE) {
    pass = false;
  }
  
  G4String process_name = "eBrem";
  //G4String process_name = "Transportation";
  
  G4String processType = aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();

  //save only bremstralung process
  if ( (processType != process_name) ) {
    pass = false;
  }
  //is the track in the volume before Ecal?
  double rInit = aStep->GetPostStepPoint()->GetPosition().perp()*sim::g42edm::length;
  double rmin = 2600;
  double zmax = 4000;  
  if ( (rInit>rmin) || (aStep->GetPostStepPoint()->GetPosition().z()*sim::g42edm::length>zmax)) {
    pass = false;
  }
  
  if (pass) {
    G4Track* aTrack = aStep->GetTrack();
    const G4DynamicParticle* dynamicparticle = aTrack->GetDynamicParticle();
    G4int pdgCode = dynamicparticle->GetPDGcode();
    
    std::cout << "Particle to be stored: PDG " << pdgCode  
	      << " name " << dynamicparticle->GetDefinition()->GetParticleName() 
	      << " track vertex r " << aTrack->GetVertexPosition().perp() 
	      << " track position r " << aTrack->GetPosition().perp()
	      << " prestep r " << aStep->GetPreStepPoint()->GetPosition().perp()
	      << " poststep r " << aStep->GetPostStepPoint()->GetPosition().perp()
	      << " trackE " <<  aTrack->GetMomentum().mag() 
	      << " track P from kinE " <<  sqrt(aTrack->GetKineticEnergy()*(aTrack->GetKineticEnergy()+aTrack->GetDynamicParticle()->GetDefinition()->GetPDGMass()))
	      << " preStepE " << aStep->GetPreStepPoint()->GetMomentum().mag()
	      << " postStepE " << aStep->GetPostStepPoint()->GetMomentum().mag()
	      << " trackID " << aTrack->GetTrackID() 
	      << " mother ID " << aTrack->GetParentID()
	      << " process " << processType 
	      << std::endl;

    const G4TrackVector& secondaries = *aStep->GetSecondary();
    secondaries_toBeStored.clear();

    std::cout << "N of secondaries: " << secondaries.size() <<  std::endl;

    for (auto iterator_sec = secondaries.begin(); iterator_sec!=secondaries.end(); iterator_sec++) {
      if ( (*iterator_sec)->GetCreatorProcess()->GetProcessName() == process_name ) { 
	  std::cout << "secondaries: PDG " << (*iterator_sec)->GetDynamicParticle()->GetPDGcode()
		    << " trackID " << (*iterator_sec)->GetTrackID() 
		    << " mother ID " << (*iterator_sec)->GetParentID()
		    << " process " << (*iterator_sec)->GetCreatorProcess()->GetProcessName()
		    << " vertex r " <<(*iterator_sec)->GetVertexPosition().perp() 
		    << " vertex z " <<(*iterator_sec)->GetVertexPosition().z()
		    << " init vertex r " <<(*iterator_sec)->GetPosition().perp() 
		    << " init vertex z " <<(*iterator_sec)->GetPosition().z()
		    << " trackE " << (*iterator_sec)->GetMomentum().mag()
		    << std::endl;
	  if ((*iterator_sec)->GetMomentum().mag() > minE_secondaries) {
	    secondaries_toBeStored.push_back( (*iterator_sec));
	  }
      }
    }
  }

 
  return pass;
}

}
