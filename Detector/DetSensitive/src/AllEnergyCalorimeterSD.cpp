#include "DetSensitive/AllEnergyCalorimeterSD.h"

// FCCSW
#include "DetCommon/DetUtils.h"

// DD4hep
#include "DDG4/Defs.h"
#include "DDG4/Geant4Mapping.h"
#include "DDG4/Geant4VolumeManager.h"

#include "G4EventManager.hh"
#include "G4SteppingManager.hh"

#include "DDG4/Factories.h"
#include "G4Version.hh"
#include "G4AdjointAlpha.hh"
#include "G4AdjointDeuteron.hh"
#include "G4AdjointElectron.hh"
#include "G4AdjointGamma.hh"
#include "G4AdjointHe3.hh"
#include "G4AdjointPositron.hh"
#include "G4AdjointProton.hh"
#include "G4AdjointTriton.hh"
#include "G4Geantino.hh"
#include "G4ChargedGeantino.hh"
#include "G4Gamma.hh"
#include "G4OpticalPhoton.hh"
#include "G4BosonConstructor.hh"
#include "G4XibZero.hh"
#include "G4SigmabPlus.hh"
#include "G4Lambda.hh"
#include "G4Proton.hh"
#include "G4SigmabMinus.hh"
#include "G4AntiXiMinus.hh"
#include "G4AntiSigmabMinus.hh"
#include "G4XiMinus.hh"
#include "G4LambdacPlus.hh"
#include "G4AntiOmegabMinus.hh"
#include "G4Neutron.hh"
#include "G4AntiXibZero.hh"
#include "G4OmegacZero.hh"
#include "G4SigmacPlus.hh"
#include "G4AntiXibMinus.hh"
#include "G4Lambdab.hh"
#include "G4AntiXicZero.hh"
#include "G4AntiOmegacZero.hh"
#include "G4AntiLambdacPlus.hh"
#include "G4AntiSigmaZero.hh"
#include "G4AntiSigmaPlus.hh"
#include "G4SigmaZero.hh"
#include "G4AntiLambda.hh"
#include "G4XiZero.hh"
#include "G4AntiSigmabZero.hh"
#include "G4AntiSigmaMinus.hh"
#include "G4AntiProton.hh"
#include "G4XicPlus.hh"
#include "G4BaryonConstructor.hh"
#include "G4AntiSigmacPlusPlus.hh"
#include "G4XibMinus.hh"
#include "G4AntiSigmabPlus.hh"
#include "G4AntiSigmacPlus.hh"
#include "G4SigmaMinus.hh"
#include "G4SigmacPlusPlus.hh"
#include "G4AntiXicPlus.hh"
#include "G4SigmabZero.hh"
#include "G4AntiXiZero.hh"
#include "G4AntiOmegaMinus.hh"
#include "G4OmegabMinus.hh"
#include "G4OmegaMinus.hh"
#include "G4AntiSigmacZero.hh"
#include "G4AntiNeutron.hh"
#include "G4SigmaPlus.hh"
#include "G4AntiLambdab.hh"
#include "G4XicZero.hh"
#include "G4SigmacZero.hh"
#include "G4AntiHe3.hh"
#include "G4Deuteron.hh"
#include "G4He3.hh"
#include "G4IonConstructor.hh"
#include "G4Alpha.hh"
#include "G4AntiAlpha.hh"
#include "G4AntiTriton.hh"
#include "G4GenericIon.hh"
#include "G4AntiDeuteron.hh"
#include "G4Triton.hh"
#include "G4Eta.hh"
#include "G4KaonMinus.hh"
#include "G4KaonZeroLong.hh"
#include "G4DsMesonPlus.hh"
#include "G4BMesonPlus.hh"
#include "G4PionPlus.hh"
#include "G4KaonZeroShort.hh"
#include "G4MesonConstructor.hh"
#include "G4DsMesonMinus.hh"
#include "G4AntiBsMesonZero.hh"
#include "G4DMesonZero.hh"
#include "G4AntiDMesonZero.hh"
#include "G4BsMesonZero.hh"
#include "G4BMesonMinus.hh"
#include "G4PionZero.hh"
#include "G4KaonPlus.hh"
#include "G4DMesonMinus.hh"
#include "G4BcMesonMinus.hh"
#include "G4BMesonZero.hh"
#include "G4AntiKaonZero.hh"
#include "G4EtaPrime.hh"
#include "G4AntiBMesonZero.hh"
#include "G4JPsi.hh"
#include "G4KaonZero.hh"
#include "G4DMesonPlus.hh"
#include "G4PionMinus.hh"
#include "G4Etac.hh"
#include "G4BcMesonPlus.hh"
#include "G4MuonPlus.hh"
#include "G4TauPlus.hh"
#include "G4AntiNeutrinoTau.hh"
#include "G4MuonMinus.hh"
#include "G4NeutrinoE.hh"
#include "G4TauMinus.hh"
#include "G4Positron.hh"
#include "G4AntiNeutrinoMu.hh"
#include "G4NeutrinoMu.hh"
#include "G4AntiNeutrinoE.hh"
#include "G4Electron.hh"
#include "G4NeutrinoTau.hh"
#include "G4LeptonConstructor.hh"
#include "G4ExcitedXiConstructor.hh"
#include "G4ExcitedLambdaConstructor.hh"
#include "G4ShortLivedConstructor.hh"
#include "G4ExcitedSigmaConstructor.hh"
#include "G4ExcitedBaryons.hh"
#include "G4DiQuarks.hh"
#include "G4VShortLivedParticle.hh"
#include "G4Quarks.hh"
#include "G4ExcitedDeltaConstructor.hh"
#include "G4ExcitedBaryonConstructor.hh"
#include "G4ExcitedNucleonConstructor.hh"
#include "G4ExcitedMesonConstructor.hh"
#include "G4ExcitedMesons.hh"


// CLHEP
#include "CLHEP/Vector/ThreeVector.h"

// Geant4
#include "G4SDManager.hh"

namespace det {
AllEnergyCalorimeterSD::AllEnergyCalorimeterSD(const std::string& aDetectorName,
                                         const std::string& aReadoutName,
                                         const dd4hep::Segmentation& aSeg)
    : G4VSensitiveDetector(aDetectorName), m_calorimeterCollection(nullptr), m_seg(aSeg) {
  // name of the collection of hits is determined byt the readout name (from XML)
  collectionName.insert(aReadoutName);
}

AllEnergyCalorimeterSD::~AllEnergyCalorimeterSD() {}

void AllEnergyCalorimeterSD::Initialize(G4HCofThisEvent* aHitsCollections) {
  // create a collection of hits and add it to G4HCofThisEvent
  // deleted in ~G4Event
  m_calorimeterCollection =
      new G4THitsCollection<dd4hep::sim::Geant4CalorimeterHit>(SensitiveDetectorName, collectionName[0]);
  aHitsCollections->AddHitsCollection(G4SDManager::GetSDMpointer()->GetCollectionID(m_calorimeterCollection),
                                      m_calorimeterCollection);
}

bool AllEnergyCalorimeterSD::ProcessHits(G4Step* aStep, G4TouchableHistory*) {
  // check if energy was deposited  
  G4double edepInvisible = 0.0;

  G4Track* pTrack = aStep->GetTrack();
  G4ParticleDefinition* particle = pTrack->GetDefinition();
  G4TrackStatus status = pTrack->GetTrackStatus();
  G4double dEStepVisible = aStep->GetTotalEnergyDeposit();
  /*
  G4int processSubTypeValue=0;
  if ( aStep->GetPostStepPoint()->GetProcessDefinedStep() != 0 ) {
    //from G4VProcess.hh
    processSubTypeValue =
      aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessSubType();
  }
  */
  G4double EkinPreStep = aStep->GetPreStepPoint()->GetKineticEnergy();
  const G4DynamicParticle* dynParticle = pTrack->GetDynamicParticle();
  G4double EkinPostStep = pTrack->GetKineticEnergy();

  // Start of calculation of invisible energy for current step.
  if ( status == fStopAndKill ){ // StopAndKill stepping particle at PostStep
    G4double incomingEkin = EkinPreStep - dEStepVisible;
    G4double incomingEtot = dynParticle->GetMass() + incomingEkin;
    edepInvisible = measurableEnergy(particle,
                          particle->GetPDGEncoding(),
                          incomingEtot,
                          incomingEkin);
   
    //edepInvisible = incomingEtot;
  }
  else if (status == fAlive || status == fStopButAlive) {
    // Alive stepping particle at PostStep
    edepInvisible = EkinPreStep - EkinPostStep - dEStepVisible;
  }
  
  G4SteppingManager* steppingManager =
        G4EventManager::GetEventManager()->GetTrackingManager()->GetSteppingManager();
 
     // Copy internal variables from the G4SteppingManager.
     G4TrackVector* fSecondary = steppingManager->GetfSecondary();
    G4int fN2ndariesAtRestDoIt = steppingManager->GetfN2ndariesAtRestDoIt();
    G4int fN2ndariesAlongStepDoIt = steppingManager->GetfN2ndariesAlongStepDoIt();
     G4int fN2ndariesPostStepDoIt = steppingManager->GetfN2ndariesPostStepDoIt();

     G4int tN2ndariesTot = fN2ndariesAtRestDoIt +
                           fN2ndariesAlongStepDoIt +
                           fN2ndariesPostStepDoIt;
 
     // loop through secondary particles which were added at current step
   // to the list of all secondaries of current track:
     G4int loopStart = (*fSecondary).size() - tN2ndariesTot;
    size_t loopEnd  = (*fSecondary).size();
     if (loopStart < 0) {
       loopEnd = loopEnd - loopStart;
       loopStart = 0;
     }
 
     G4ParticleDefinition *secondaryID;
     G4double totalEofSecondary=0, kinEofSecondary=0, measurEofSecondary=0;

     for(size_t lp1=loopStart; lp1<loopEnd; lp1++) {

       //----- extract information about each new secondary particle:
       secondaryID = (*fSecondary)[lp1]->GetDefinition();
       totalEofSecondary = (*fSecondary)[lp1]->GetTotalEnergy();
 
       //----- use this information:
      if (secondaryID == G4NeutrinoE::Definition() || secondaryID == G4AntiNeutrinoE::Definition() || // nu_e,   anti_nu_e
           secondaryID == G4NeutrinoMu::Definition() || secondaryID == G4AntiNeutrinoMu::Definition() || // nu_mu,  anti_nu_mu
           secondaryID == G4NeutrinoTau::Definition() || secondaryID == G4AntiNeutrinoTau::Definition())   // nu_tau, anti_nu_tau
       {
	 //Neutrinos to be counted as invisible 
         //edepInvisible -= totalEofSecondary;
         //result.energy[kEscaped] += totalEofSecondary;
     }
     else {
         //----- extract further information about each new secondary particle:
         kinEofSecondary = (*fSecondary)[lp1]->GetKineticEnergy();
         measurEofSecondary = measurableEnergy(secondaryID,
                                              secondaryID->GetPDGEncoding(),
                                               totalEofSecondary,
                                               kinEofSecondary);
         edepInvisible -= measurEofSecondary;
      }
    }
 

  if ( (edepInvisible + dEStepVisible) == 0.) return false;

  // as in dd4hep::sim::Geant4GenericSD<Calorimeter>
  CLHEP::Hep3Vector prePos = aStep->GetPreStepPoint()->GetPosition();
  dd4hep::Position pos(prePos.x(), prePos.y(), prePos.z());
  auto hit = new dd4hep::sim::Geant4CalorimeterHit(pos);
  hit->cellID = utils::cellID(m_seg, *aStep);
  hit->energyDeposit = dEStepVisible;
  int trackID = -1;  
  int pdgID = -1;
  double deposit = dEStepVisible + edepInvisible;
  double time = 0.0;
  
  dd4hep::sim::Geant4Hit::MonteCarloContrib mc_contrib(trackID, pdgID, deposit, time);
  hit->truth.push_back(mc_contrib);      
  m_calorimeterCollection->insert(hit);
  return true;
}

  // Output: measurableEnergy - energy measurable in a calorimeter
G4double AllEnergyCalorimeterSD::measurableEnergy(const G4ParticleDefinition* particleDef,
						  G4int PDGEncoding,
						  G4double totalEnergy,
						  G4double kineticEnergy) const
{
  const G4double electronMass = G4Electron::Definition()->GetPDGMass();
  const G4double protonMass = G4Proton::Definition()->GetPDGMass();
  const G4double neutronMass = G4Neutron::Definition()->GetPDGMass();
  const G4double pionMass = G4PionPlus::Definition()->GetPDGMass();
  
  G4double correctionForMass = 0.;
  if (particleDef == G4Electron::Definition()){
    correctionForMass = - electronMass;
  }
  else if (particleDef == G4Positron::Definition()){
    correctionForMass = + electronMass;
  }
  else if (particleDef == G4Proton::Definition()){
    correctionForMass = - protonMass;
  }
  else if (particleDef == G4Neutron::Definition()){
    correctionForMass = - neutronMass;
  }
  //JANA - -minus pion mass
  //else if (particleDef == G4PionPlus::Definition()){
  //  correctionForMass = - pionMass;
  //}
  //else if (particleDef == G4PionMinus::Definition()){
  //  correctionForMass = - pionMass;
  //}
  else if (PDGEncoding > 1000010019 ){ //for nuclei
    return kineticEnergy;
  }
  else if (particleDef == G4Lambda::Definition() ||
	   particleDef == G4SigmaPlus::Definition() ||
	   particleDef == G4SigmaZero::Definition() ||
	   particleDef == G4XiMinus::Definition() ||
	   particleDef == G4XiZero::Definition() ||
	   particleDef == G4OmegaMinus::Definition()) {
    correctionForMass = - protonMass;
  }
  else if (particleDef == G4SigmaMinus::Definition()) { // Need Sigma Minus?
    correctionForMass = - neutronMass;
  }
  
  else if (particleDef == G4AntiNeutron::Definition()) { 
    correctionForMass = + neutronMass;
  }
  else if (particleDef == G4AntiProton::Definition() ||
	   particleDef == G4AntiLambda::Definition() ||
	   particleDef == G4AntiSigmaZero::Definition() ||
	   // Need AntiSigmacPlus and Zero as well?
           particleDef == G4AntiSigmaPlus::Definition() ||
	   particleDef == G4AntiXiZero::Definition() ||
	   // Need AntiXicMinus and Zero as well?
	   particleDef == G4AntiXiMinus::Definition() ||
	   particleDef == G4AntiOmegaMinus::Definition()) {
    correctionForMass = + protonMass;
  }
  else if (particleDef == G4AntiSigmaMinus::Definition()) {
    correctionForMass = + neutronMass;
  }
  G4double measurableEnergy = totalEnergy + correctionForMass;
  if (measurableEnergy < -0.0001) {
    G4cerr << "Calibration Hit: NEGATIVE measurableEnergy="
	   << measurableEnergy << " < -0.0001 MeV for "
	   << particleDef->GetParticleName() << G4endl;
    measurableEnergy = 0.;
  }
  return measurableEnergy;
}
}
