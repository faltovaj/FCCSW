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

namespace sim {
MCTruthSteppingAction::MCTruthSteppingAction():
  G4UserSteppingAction() {}

MCTruthSteppingAction::~MCTruthSteppingAction() {}

void MCTruthSteppingAction::UserSteppingAction(const G4Step* aStep) {
    
  G4Track* aTrack = aStep->GetTrack();
  /*
  aInitMom.setPx( aTrack->GetMomentum().x() );
  aInitMom.setPy( aTrack->GetMomentum().y() );
  aInitMom.setPz( aTrack->GetMomentum().z() );
  aInitMom.setE( aTrack->GetTotalEnergy() );
  */

  /*
  if(!aTrack->GetUserInformation()) 
    {
      MCTruthTrackInformation* mcinf = new MCTruthTrackInformation;
    
      fpTrackingManager->SetUserTrackInformation(mcinf);
    }
  */
  
  /*
  // make all different checks to decide whether to store the particle
  prodPosition = (aTrack->GetGlobalTime() - aTrack->GetLocalTime(), aTrack->GetVertexPosition());
  endPosition = (aTrack->GetGlobalTime(), aTrack->GetPosition());
  //TODO: Time in which units? mm?                                                              
  */

  aInitMom = (aStep->GetPreStepPoint()->GetTotalEnergy(), aStep->GetPreStepPoint()->GetMomentum());

  prodPosition = (0,aStep->GetPreStepPoint()->GetPosition());    
  endPosition = (0,aStep->GetPostStepPoint()->GetPosition());

  statusTrack = aTrack->GetTrackStatus(); //TrackStatus is enum type

  if (trackToBeStored(aStep))
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

    G4String processType = aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
    /*
    std::cout << "Stored particle: PDG " << pdgCode  
	      << " name " << dynamicparticle->GetDefinition()->GetParticleName() 
	      << " vertex r " << prodPosition.perp() 
	      << " vertex z " << prodPosition.z()
	      << " end z " << endPosition.z() 
	      << " kinE " << aInitMom.mag()
	      << " trackID " << aTrack->GetTrackID() 
	      << " mother ID " << aTrack->GetParentID()
	      << " process " << processType 
	      << std::endl;
    */
   /*
    if (aTrack->GetParentID()!=0) {
      std::cout << " process " << processType << std::endl;
      //std::cout << " step process pre " << aStep->GetPreStepPoint()->GetProcessDefinedStep()->GetProcessName();
    }
   */
    //std::cout << " step process post " << aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() << std::endl;
    }
  else
  {
    // If track is not to be stored, propagate it's parent ID (stored)
    // to its secondaries
    //
    /*
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
    */    
  }


  return;
}

bool MCTruthSteppingAction::trackToBeStored(const G4Step* aStep)
{

  bool pass = true;  

  double MinE = 1;
  double MinE_eioni = 1;
  // check energy
  //double kinE = sqrt(std::pow(aInitMom.x(),2)+std::pow(aInitMom.y(),2)+std::pow(aInitMom.z(),2)); 
  double kinE = aStep->GetPostStepPoint()->GetMomentum().mag();
  double trackKinE = aStep->GetPreStepPoint()->GetMomentum().perp();

  if (trackKinE < MinE) {
    // std::cout << "Track accepted px " << aInitMom.x() << " py " << aInitMom.y() << " pz " << aInitMom.z() << std::endl;
    pass = false;
  }
  
  
  G4String process_name = "eBrem";
  
  G4String processType = aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();

  //if (aTrack->GetParentID()!=0) {
  /*
  if ((processType == "eIoni") || (processType == "Transportation") ) {
    pass = false;
  }
  */
  //}

  if (processType != process_name) {
    pass = false;
  }
  //where is the track?
  double rInit = aStep->GetPostStepPoint()->GetPosition().perp()*sim::g42edm::length;
  double rmin = 2600;
  double zmax = 4000;  
  if ( (rInit>rmin) || (aStep->GetPostStepPoint()->GetPosition().z()*sim::g42edm::length>zmax)) {
    pass = false;
  }
  else {
    // if (kinE>MinE) std::cout << "rInit " << rInit << " z " << prodPosition.z()*sim::g42edm::length << std::endl;
  }
  
  if (pass) {
    G4Track* aTrack = aStep->GetTrack();
    const G4DynamicParticle* dynamicparticle = aTrack->GetDynamicParticle();
    pdgCode = dynamicparticle->GetPDGcode();
    
    std::cout << "Particle to be stored: PDG " << pdgCode  
	      << " name " << dynamicparticle->GetDefinition()->GetParticleName() 
      // << " vertex r " << prodPosition.perp() 
      //      << " vertex z " << prodPosition.z()
	      << " end r " << endPosition.perp() 
	      << " end z " << endPosition.z() 
	      << " kinE " << aInitMom.mag()
	      << " trackID " << aTrack->GetTrackID() 
	      << " mother ID " << aTrack->GetParentID()
	      << " process " << processType 
	      << std::endl;

    const G4TrackVector& secondaries = *aStep->GetSecondary();

    for (auto iterator_sec = secondaries.begin(); iterator_sec!=secondaries.end(); iterator_sec++) {
      if ((*iterator_sec)->GetCreatorProcess()->GetProcessName() == process_name) { 
	std::cout << "secondaries: PDG " << (*iterator_sec)->GetDynamicParticle()->GetPDGcode()
		  << " trackID " << (*iterator_sec)->GetTrackID() 
		  << " mother ID " << (*iterator_sec)->GetParentID()
		<< " process " << (*iterator_sec)->GetCreatorProcess()->GetProcessName()
		  << " vertex r " <<(*iterator_sec)->GetPosition().perp() 
		  << " vertex z " <<(*iterator_sec)->GetPosition().z()
		  << " kinE " << (*iterator_sec)->GetMomentum().mag()
		  << std::endl;
      }
    }
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
