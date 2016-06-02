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
 
  if(!aTrack->GetUserInformation()) 
    {
      MCTruthTrackInformation* mcinf = new MCTruthTrackInformation;
    
      fpTrackingManager->SetUserTrackInformation(mcinf);
    }
  
  return;
}

void MCTruthTrackingAction::PostUserTrackingAction(const G4Track* aTrack) {

  // make all different checks to decide whether to store the particle
  prodPosition = (aTrack->GetGlobalTime() - aTrack->GetLocalTime(), aTrack->GetVertexPosition());
  endPosition = (aTrack->GetGlobalTime(), aTrack->GetPosition());
  //TODO: Time in which units? mm?                                                              

  statusTrack = aTrack->GetTrackStatus(); //TrackStatus is enum type

  if (trackToBeStored(aTrack))
    {

    const G4DynamicParticle* dynamicparticle = aTrack->GetDynamicParticle();
    pdgCode = dynamicparticle->GetPDGcode();
    charge = dynamicparticle->GetDefinition()->GetPDGCharge();
    status = 1e6;  //TODO: define the status correctly

    MCTruthTrackInformation* mcinf = (MCTruthTrackInformation*) aTrack->GetUserInformation();
    const G4Event *aEvent = (G4EventManager::GetEventManager())->GetConstCurrentEvent();
    MCTruthEventInformation* mcevinf = (MCTruthEventInformation*) aEvent->GetUserInformation();
    mcevinf->AddParticle(aInitMom, prodPosition, endPosition, pdgCode, charge, status);

    /*
    MCTruthManager::GetInstance()->
      AddParticle(fmom, prodpos, endpos,
                  aTrack->GetDefinition()->GetPDGEncoding(),
                  aTrack->GetTrackID(),
                  aTrack->GetParentID(), mcinf->GetDirectParent());
    */

    std::cout << "Stored particle: PDG " << pdgCode  << " name " << dynamicparticle->GetDefinition()->GetParticleName() << " vertex r " << sqrt(pow(prodPosition.x(),2)+pow(prodPosition.x(),2)) << " vertex z " << prodPosition.z() << " kinE " << sqrt(std::pow(aInitMom.x(),2)+std::pow(aInitMom.y(),2)+std::pow(aInitMom.z(),2)) << " trackID " << aTrack->GetTrackID() << " mother ID " << aTrack->GetParentID();
    if (aTrack->GetParentID()!=0) {
      std::cout << " process " << aTrack->GetCreatorProcess()->GetProcessName() << std::endl;
    }
    std::cout << std::endl;
  std::cout << "step prestep z " << aTrack->GetStep()->GetPreStepPoint()->GetPosition().z() << " poststep z " << aTrack->GetStep()->GetPostStepPoint()->GetPosition().z() << std::endl;
    
  /*                                                                                                                                
  G4TrackVector* secondaries = fpTrackingManager->GimmeSecondaries() ;

  for( unsigned int index = 0 ; index < secondaries->size() ; ++index )
    {
      G4Track* sTr = (*secondaries)[index] ;

      prodPosition = (sTr->GetGlobalTime() - sTr->GetLocalTime(), sTr->GetVertexPosition());
      endPosition = (sTr->GetGlobalTime(), sTr->GetPosition());

      std::cout << "=== Secondaries PDG " <<  sTr->GetDynamicParticle()->GetPDGcode() << " prod z  " << prodPosition.z()  << " end z " << endPosition.z() << std::endl;

    }
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

  bool pass = true;  

  double MinE = -1000;
  double MinE_eioni = 1;
  // check energy
  double kinE = sqrt(std::pow(aInitMom.x(),2)+std::pow(aInitMom.y(),2)+std::pow(aInitMom.z(),2)); 
 
  if (kinE < MinE) {
    // std::cout << "Track accepted px " << aInitMom.x() << " py " << aInitMom.y() << " pz " << aInitMom.z() << std::endl;
    pass = false;
  }
  

  G4String process_name = "eIoni";
  if (aTrack->GetParentID()!=0) {
    if ((aTrack->GetCreatorProcess()->GetProcessName()==process_name) && (kinE<MinE_eioni)) {
      pass = false;
    }
    if (aTrack->GetCreatorProcess()->GetProcessName()=="annihil") {
      pass = false;
    }
  }

  //where is the track?
  double rInit = sqrt( pow(prodPosition.x()*sim::g42edm::length,2)+pow(prodPosition.y()*sim::g42edm::length,2) );
  double rmin = 2600;
  double zmax = 4000;  
  if ( (rInit>rmin) || (prodPosition.z()*sim::g42edm::length>zmax)) {
    pass = false;
  }
  else {
    // if (kinE>MinE) std::cout << "rInit " << rInit << " z " << prodPosition.z()*sim::g42edm::length << std::endl;
  }

  /*
  //which volume?
  if (aTrack->GetMaterial()->GetName()!="Air") {
    pass = false;
  }
  else {
    std::cout << "In Air" << std::endl;
  }
  */

  /*
  // particle type
  std::vector<G4int> types = config->GetParticleTypes();
  
  if(std::find( types.begin(), types.end(),
                track->GetDefinition()->GetPDGEncoding())
     != types.end()) return true;

  // creator process
  */
  // etc...
  
  return pass;
}

}
