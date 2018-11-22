#include "ShowerConeSize.h"

// FCCSW
#include "DetInterface/IGeoSvc.h"
#include "DetSegmentation/FCCSWGridPhiEta.h"

// datamodel
#include "datamodel/CaloHitCollection.h"   
#include "datamodel/MCParticleCollection.h"

#include "CLHEP/Vector/ThreeVector.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TVector2.h"
#include "TVector3.h"
#include "TH1F.h"
#include "TH2F.h"

// DD4hep
#include "DD4hep/Detector.h"
#include "DD4hep/Readout.h"

#include <math.h>
#include <iostream>

DECLARE_ALGORITHM_FACTORY(ShowerConeSize)

ShowerConeSize::ShowerConeSize(const std::string& aName, ISvcLocator* aSvcLoc)
    : GaudiAlgorithm(aName, aSvcLoc),
      m_histSvc("THistSvc", "ShowerConeSize"),
  m_geoSvc("GeoSvc", "ShowerConeSize") {
  declareProperty("ecal_cells", m_ecal_cells, "ECAL cells (input)");
  declareProperty("hcal_cells", m_hcal_cells, "HCAL cells (input)");
  declareProperty("particles", m_particles, "MC Particles (input)");
}
ShowerConeSize::~ShowerConeSize() {}

StatusCode ShowerConeSize::initialize() {
  if (GaudiAlgorithm::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  // check if readouts exist
  if (m_geoSvc->lcdd()->readouts().find(m_readoutNameEcal) == m_geoSvc->lcdd()->readouts().end()) {
    error() << "Readout <<" << m_readoutNameEcal << ">> does not exist." << endmsg;
    return StatusCode::FAILURE;
  }
  m_ecalBarrelSegmentation = dynamic_cast<dd4hep::DDSegmentation::FCCSWGridPhiEta*>(m_geoSvc->lcdd()->readout(m_readoutNameEcal).segmentation().segmentation());
  if (m_ecalBarrelSegmentation == nullptr) {
    error() << "There is no phi-eta segmentation for the readout " << m_readoutNameEcal << " defined." << endmsg;
    return StatusCode::FAILURE;
  }
  if (m_geoSvc->lcdd()->readouts().find(m_readoutNameHcal) == m_geoSvc->lcdd()->readouts().end()) {
    error() << "Readout <<" << m_readoutNameHcal << ">> does not exist." << endmsg;
    return StatusCode::FAILURE;
  }
  m_hcalBarrelSegmentation = dynamic_cast<dd4hep::DDSegmentation::FCCSWGridPhiEta*>(m_geoSvc->lcdd()->readout(m_readoutNameHcal).segmentation().segmentation());
  if (m_hcalBarrelSegmentation == nullptr) {
    error() << "There is no phi-eta segmentation for the readout " << m_readoutNameHcal << " defined." << endmsg;
    return StatusCode::FAILURE;
  }
  
  // create histograms
  m_totalEnergy = new TH1F("totalEnergy", "Total energy", 500, 0.0, 1.2 * m_beamEnergy);
  if (m_histSvc->regHist("/rec/totalEnergy", m_totalEnergy).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_totalEnergy_ecal = new TH1F("totalEnergy_ecal", "Total energy in ECAL", 500, 0.0, 1.2 * m_beamEnergy);
  if (m_histSvc->regHist("/rec/totalEnergy_ecal", m_totalEnergy_ecal).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_totalEnergy_hcal = new TH1F("totalEnergy_hcal", "Total energy in HCAL", 500, 0.0, 1.2 * m_beamEnergy);
  if (m_histSvc->regHist("/rec/totalEnergy_hcal", m_totalEnergy_hcal).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_eneFraction_ecal = new TH1F("eneFraction_ecal", "energy fraction in DeltaR in ECAL", 3000, 0.8, 1.1);
  if (m_histSvc->regHist("/rec/eneFraction_ecal", m_eneFraction_ecal).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_eneFraction_hcal = new TH1F("eneFraction_hcal", "energy fraction in DeltaR in HCAL", 3000, 0.8, 1.1);
  if (m_histSvc->regHist("/rec/eneFraction_hcal", m_eneFraction_hcal).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }

  m_deltaEta_ecal = new TH1F("deltaEta_ecal", "Max. eta distance from shower center in ECAL", 200, 0.0, 5.0);
  if (m_histSvc->regHist("/rec/deltaEta_ecal", m_deltaEta_ecal).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_deltaPhi_ecal = new TH1F("deltaPhi_ecal", "Max. phi distance from shower center in ECAL", 200, 0.0, 5.0);
  if (m_histSvc->regHist("/rec/deltaPhi_ecal", m_deltaPhi_ecal).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_deltaR_ecal = new TH1F("deltaR_ecal", "deltaR shower dimension in ECAL", 250, 0.0, 2.5);
  if (m_histSvc->regHist("/rec/deltaR_ecal", m_deltaR_ecal).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_deltaRAvr_ecal = new TH1F("deltaRAvr_ecal", "Avr. deltaR distance in ECAL", 250, 0.0, 2.5);
  if (m_histSvc->regHist("/rec/deltaRAvr_ecal", m_deltaRAvr_ecal).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }

  m_deltaEta_hcal = new TH1F("deltaEta_hcal", "Max. eta distance from shower center in HCAL", 200, 0.0, 5.0);
  if (m_histSvc->regHist("/rec/deltaEta_hcal", m_deltaEta_hcal).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_deltaPhi_hcal = new TH1F("deltaPhi_hcal", "Max. phi distance from shower center in HCAL", 200, 0.0, 5.0);
  if (m_histSvc->regHist("/rec/deltaPhi_hcal", m_deltaPhi_hcal).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_deltaR_hcal = new TH1F("deltaR_hcal", "Max. deltaR distance in HCAL", 250, 0.0, 2.5);
  if (m_histSvc->regHist("/rec/deltaR_hcal", m_deltaR_hcal).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
 
  return StatusCode::SUCCESS;
}

StatusCode ShowerConeSize::execute() {
  
  double sumE_ecal = 0.;
  double sumE_hcal = 0.;

  TH1F *radius_ecal = new TH1F("radius_ecal", "Cell energy vs. radius in ECAL", 500, 0.0, 5.);
  TH1F *radius_hcal = new TH1F("radius_hcal", "Cell energy vs. radius in HCAL", 500, 0.0, 5.);

  double eta, phi;
  double deltaEta = 0;
  double deltaPhi = 0;
  double deltaR = 0;
  double ene = 0;
  double meanPhi = 0;
  double meanEta = 0;
 

  const auto mcparticles = m_particles.get();
  for (const auto& part : *mcparticles) {
    TVector3 p(part.p4().px,part.p4().py,part.p4().pz);
    meanPhi = p.Phi();
    meanEta = p.Eta();
  }

  const auto ecal_cells = m_ecal_cells.get();
  for (const auto& hit : *ecal_cells) {
    ene = hit.core().energy;
    sumE_ecal += ene;
    eta = m_ecalBarrelSegmentation->eta( hit.core().cellId );
    phi = m_ecalBarrelSegmentation->phi( hit.core().cellId );
    if (ene>7.) {
      debug() << ene << " " << eta << " " << phi << " " << meanEta << " " << meanPhi << endmsg;
    }
    deltaEta = eta-meanEta;
    deltaPhi = TVector2::Phi_mpi_pi(phi-meanPhi);
    deltaR = sqrt(deltaEta*deltaEta+deltaPhi*deltaPhi);
    radius_ecal->Fill(deltaR, ene);
  }
  radius_ecal->Scale(1./sumE_ecal);

  for (const auto& hit : *ecal_cells) {
    ene = hit.core().energy;
    eta = m_ecalBarrelSegmentation->eta( hit.core().cellId );
    phi = m_ecalBarrelSegmentation->phi( hit.core().cellId );
    deltaEta = eta-meanEta;
    deltaPhi = TVector2::Phi_mpi_pi(phi-meanPhi);
    deltaR = sqrt(deltaEta*deltaEta+deltaPhi*deltaPhi);
    m_deltaRAvr_ecal->Fill(deltaR, ene/sumE_ecal);
  }

  const auto hcal_cells = m_hcal_cells.get();
  for (const auto& hit : *hcal_cells) {
    ene = hit.core().energy;
    sumE_hcal += hit.core().energy;
    eta = m_hcalBarrelSegmentation->eta( hit.core().cellId );
    phi = m_hcalBarrelSegmentation->phi( hit.core().cellId );
    deltaEta = eta-meanEta;
    deltaPhi = TVector2::Phi_mpi_pi(phi-meanPhi);
    deltaR = sqrt(deltaEta*deltaEta+deltaPhi*deltaPhi);
    radius_hcal->Fill(deltaR, ene);
  }
  radius_hcal->Scale(1./sumE_hcal);

  //debug() << "Total E: in ECAL " << sumE_ecal << " in HCAL " << sumE_hcal << endmsg;

  m_totalEnergy_ecal->Fill(sumE_ecal);
  m_totalEnergy_hcal->Fill(sumE_hcal);
  m_totalEnergy->Fill(sumE_ecal + sumE_hcal);

  double radius_max_ecal = 0;
  double radius_max_hcal = 0;
  double integral = 0;


  if (sumE_ecal>0) {
    for (int i = 1; i<radius_ecal->GetNbinsX()+1;i++) {
      integral = radius_ecal->Integral(0,i);
      if (integral >= m_containment) {
	radius_max_ecal = radius_ecal->GetBinLowEdge(i);
	m_deltaR_ecal->Fill(radius_max_ecal);
	m_eneFraction_ecal->Fill(radius_ecal->Integral(0,i-1)/sumE_ecal);
	break;
      }
    }
  }
  if (sumE_hcal>0) {
    for (int i = 1; i<radius_hcal->GetNbinsX()+1;i++) {
      integral = radius_hcal->Integral(0,i);
      if (integral >= m_containment) {
	radius_max_hcal = radius_hcal->GetBinLowEdge(i);
	m_deltaR_hcal->Fill(radius_max_hcal);
	m_eneFraction_hcal->Fill(radius_hcal->Integral(0,i-1)/sumE_hcal);
	break;
      }
    }
  }
 //Tests
 double eMax_eta = 0;
 double eMax_phi = 0;
  for (const auto& hit : *ecal_cells) {    
    eta = m_ecalBarrelSegmentation->eta( hit.core().cellId );
    phi = m_ecalBarrelSegmentation->phi( hit.core().cellId );
    deltaEta = eta-meanEta;
    deltaPhi = TVector2::Phi_mpi_pi(phi-meanPhi);
    deltaR = sqrt(deltaEta*deltaEta+deltaPhi*deltaPhi);
    if (deltaR<radius_max_ecal) {
	if ( (eta-meanEta)>eMax_eta ) {
	  eMax_eta = eta-meanEta;
	}
	if ( TVector2::Phi_mpi_pi(phi-meanPhi)>eMax_phi ) {
	  eMax_phi = TVector2::Phi_mpi_pi(phi-meanPhi);
	}
    }
  }
  if (sumE_ecal>0) {
    m_deltaEta_ecal->Fill(eMax_eta);      
    m_deltaPhi_ecal->Fill(eMax_phi);
  }

  eMax_eta = 0;
  eMax_phi = 0;
  for (const auto& hit : *hcal_cells) {    
    eta = m_hcalBarrelSegmentation->eta( hit.core().cellId );
    phi = m_hcalBarrelSegmentation->phi( hit.core().cellId );
    deltaEta = eta-meanEta;
    deltaPhi = TVector2::Phi_mpi_pi(phi-meanPhi);
    deltaR = sqrt(deltaEta*deltaEta+deltaPhi*deltaPhi);
    if (deltaR<radius_max_hcal) {
	if ( (eta-meanEta)>eMax_eta ) {
	  eMax_eta = eta-meanEta;
	}
	if ( TVector2::Phi_mpi_pi(phi-meanPhi)>eMax_phi ) {
	  eMax_phi = TVector2::Phi_mpi_pi(phi-meanPhi);
	}
    }
  }
  if (sumE_hcal>0) {
    m_deltaEta_hcal->Fill(eMax_eta);      
    m_deltaPhi_hcal->Fill(eMax_phi);
  }

  delete radius_ecal;
  delete radius_hcal;

  return StatusCode::SUCCESS;
}

StatusCode ShowerConeSize::finalize() { 
  m_deltaRAvr_ecal->Scale(1./(double)m_deltaR_ecal->GetEntries());
  return GaudiAlgorithm::finalize(); 
}
