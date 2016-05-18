#include "SimG4Full/MCTruthTrackingAction.h"

//FCCSW
#include "SimG4Common/Units.h"
#include "SimG4Common/MCTruthTrackInformation.h"
#include "SimG4Common/MCTruthEventInformation.h"

// datamodel
#include "datamodel/MCParticle.h"

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
    
  G4LorentzVector fmom = G4LorentzVector(aTrack->GetMomentum(), aTrack->GetTotalEnergy());

  //Primary particle: Track Id == 1
 
  if(!aTrack->GetUserInformation()) 
    {
      MCTruthTrackInformation* mcinf = new MCTruthTrackInformation;
    
      mcinf->SetInitMomentum(aTrack->GetMomentum());
      mcinf->SetInitVertexPosition(aTrack->GetVertexPosition());
      
      fpTrackingManager->SetUserTrackInformation(mcinf);
    }
  
  return;
}

void MCTruthTrackingAction::PostUserTrackingAction(const G4Track* aTrack) {

  G4LorentzVector prodpos(aTrack->GetGlobalTime() - aTrack->GetLocalTime(),
                             aTrack->GetVertexPosition());
  G4LorentzVector endpos(aTrack->GetGlobalTime(), aTrack->GetPosition());

  // here (?) make all different checks to decide whether to store the particle
  //
  if (trackToBeStored(aTrack))
  {
    MCTruthTrackInformation* mcinf = (MCTruthTrackInformation*) aTrack->GetUserInformation();
    const G4Event *aEvent = (G4EventManager::GetEventManager())->GetConstCurrentEvent();
    MCTruthEventInformation* mcevinf = (MCTruthEventInformation*) aEvent->GetUserInformation();
    mcevinf->AddParticle(aTrack->GetMomentum());

    /*
    MCTruthManager::GetInstance()->
      AddParticle(fmom, prodpos, endpos,
                  aTrack->GetDefinition()->GetPDGEncoding(),
                  aTrack->GetTrackID(),
                  aTrack->GetParentID(), mcinf->GetDirectParent());
    */
  }
  else
  {
    // If track is not to be stored, propagate it's parent ID (stored)
    // to its secondaries
    //
    G4TrackVector* childrens = fpTrackingManager->GimmeSecondaries() ;

    for( unsigned int index = 0 ; index < childrens->size() ; ++index )
    {
      G4Track* tr = (*childrens)[index] ;
      tr->SetParentID( aTrack->GetParentID() );

      // set the flag saying that the direct mother is not stored
      //
      MCTruthTrackInformation* mcinf =
        (MCTruthTrackInformation*) tr->GetUserInformation();
      if(!mcinf) {
	mcinf = new MCTruthTrackInformation;
        tr->SetUserInformation(mcinf);
      }
      //  mcinf->SetDirectParent(false); 
    }     
  }


  return;
}

bool MCTruthTrackingAction::trackToBeStored(const G4Track* aTrack)
{
  //MCTruthConfig* config = MCTruthManager::GetInstance()->GetConfig();

  double MinE = 1000;
  // check energy
  if (aTrack->GetTotalEnergy() > MinE) {
    std::cout << "Track accepted" << std::endl;
    return true;
  }
  
  /*
  // particle type
  std::vector<G4int> types = config->GetParticleTypes();
  
  if(std::find( types.begin(), types.end(),
                track->GetDefinition()->GetPDGEncoding())
     != types.end()) return true;

  // creator process
  */
  // etc...
  
  return false;
}

}
