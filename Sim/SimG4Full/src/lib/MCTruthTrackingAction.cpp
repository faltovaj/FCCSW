#include "SimG4Full/MCTruthTrackingAction.h"

//FCCSW
#include "SimG4Common/Units.h"
#include "SimG4Common/MCTruthTrackInformation.h"
#include "SimG4Common/MCTruthEventInformation.h"

// Geant4
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrackingManager.hh"
#include "G4PrimaryParticle.hh"
#include "G4ThreeVector.hh"
#include "G4VUserTrackInformation.hh"

namespace sim {
MCTruthTrackingAction::MCTruthTrackingAction():
  G4UserTrackingAction() {}

MCTruthTrackingAction::~MCTruthTrackingAction() {}

void MCTruthTrackingAction::PreUserTrackingAction(const G4Track* aTrack) {
    
  aInitMom.setPx( aTrack->GetMomentum().x() );
  aInitMom.setPy( aTrack->GetMomentum().y() );
  aInitMom.setPz( aTrack->GetMomentum().z() );
  aInitMom.setE( aTrack->GetTotalEnergy() );
  
  return;

}

void MCTruthTrackingAction::PostUserTrackingAction(const G4Track* aTrack) {

  //Save primary particle
  if ( (aTrack->GetTrackID()==1) && (aTrack->GetParentID()==0) ) 
    {

      const G4DynamicParticle* dynamicparticle = aTrack->GetDynamicParticle();
      pdgCode = dynamicparticle->GetPDGcode();
      charge = dynamicparticle->GetDefinition()->GetPDGCharge();
      status = 1;  //status 1 for primary track
      
      const G4Event *aEvent = (G4EventManager::GetEventManager())->GetConstCurrentEvent();
      MCTruthEventInformation* mcevinf = (MCTruthEventInformation*) aEvent->GetUserInformation();
      mcevinf->AddParticle(aTrack, aInitMom, status);
      
      prodPosition = (aTrack->GetGlobalTime() - aTrack->GetLocalTime(), aTrack->GetVertexPosition());
      endPosition = (aTrack->GetGlobalTime(), aTrack->GetPosition());
      
      std::cout << "Primary particle: PDG " << pdgCode  << " name " << dynamicparticle->GetDefinition()->GetParticleName() << " vertex r " << sqrt(pow(aTrack->GetVertexPosition().x(),2)+pow(aTrack->GetVertexPosition().y(),2)) << " vertex z " << aTrack->GetVertexPosition() .z() << " impuls " << sqrt(std::pow(aInitMom.x(),2)+std::pow(aInitMom.y(),2)+std::pow(aInitMom.z(),2)) << " trackID " << aTrack->GetTrackID() << " mother ID " << aTrack->GetParentID();
 
    }


  return;
}

}
