#include "ShowerConeSize.h"

// FCCSW
#include "DetInterface/IGeoSvc.h"
#include "DetSegmentation/FCCSWGridPhiEta.h"

// datamodel
#include "datamodel/CaloHitCollection.h"   

#include "CLHEP/Vector/ThreeVector.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TVector2.h"
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
  m_cellEnergy_ecal = new TH1F("cellEnergy_ecal", "Cell energy in ECAL", 1000000, 0.0, 0.5*m_beamEnergy);
  if (m_histSvc->regHist("/rec/cellEnergy_ecal", m_cellEnergy_ecal).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_cellEnergy_hcal = new TH1F("cellEnergy_hcal", "Cell energy in HCAL", 1000000, 0.0, 0.5*m_beamEnergy);
  if (m_histSvc->regHist("/rec/cellEnergy_hcal", m_cellEnergy_hcal).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }

  m_ecut_ecal = new TH1F("ecut_ecal", "energy cut in ECAL", 500, 0.0, 0.01 * m_beamEnergy);
  if (m_histSvc->regHist("/rec/ecut_ecal", m_ecut_ecal).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_ecut_hcal = new TH1F("ecut_hcal", "energy cut in HCAL", 500, 0.0, 0.01 * m_beamEnergy);
  if (m_histSvc->regHist("/rec/ecut_hcal", m_ecut_hcal).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  
  m_eneFraction_ecal = new TH1F("eneFraction_ecal", "energy fraction in high energy cells", 3000, 0.8, 1.1);
  if (m_histSvc->regHist("/rec/eneFraction_ecal", m_eneFraction_ecal).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_eneFraction_hcal = new TH1F("eneFraction_hcal", "energy fraction in high energy cells", 3000, 0.8, 1.1);
  if (m_histSvc->regHist("/rec/eneFraction_hcal", m_eneFraction_hcal).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }

  m_deltaEta_ecal = new TH1F("deltaEta_ecal", "Max. eta distance from shower center in ECAL", 500, -5.0, 5.0);
  if (m_histSvc->regHist("/rec/deltaEta_ecal", m_deltaEta_ecal).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_deltaPhi_ecal = new TH1F("deltaPhi_ecal", "Max. phi distance from shower center in ECAL", 1000, -10.0, 10.0);
  if (m_histSvc->regHist("/rec/deltaPhi_ecal", m_deltaPhi_ecal).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_deltaR_ecal = new TH1F("deltaR_ecal", "Max. deltaR distance in ECAL", 1000, -10.0, 10.0);
  if (m_histSvc->regHist("/rec/deltaR_ecal", m_deltaR_ecal).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }

  m_deltaEta_hcal = new TH1F("deltaEta_hcal", "Max. eta distance from shower center in HCAL", 500, -5.0, 5.0);
  if (m_histSvc->regHist("/rec/deltaEta_hcal", m_deltaEta_hcal).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_deltaPhi_hcal = new TH1F("deltaPhi_hcal", "Max. phi distance from shower center in HCAL", 1000, -10.0, 10.0);
  if (m_histSvc->regHist("/rec/deltaPhi_hcal", m_deltaPhi_hcal).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_deltaR_hcal = new TH1F("deltaR_hcal", "Max. deltaR distance in HCAL", 1000, -10.0, 10.0);
  if (m_histSvc->regHist("/rec/deltaR_hcal", m_deltaR_hcal).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
    
  return StatusCode::SUCCESS;
}

StatusCode ShowerConeSize::execute() {
  
  double sumE_ecal = 0.;
  double sumE_hcal = 0.;
  double sumE_high_ecal = 0.;
  double sumE_high_hcal = 0.;

  double eta, phi;
  double deltaEta = 0;
  double deltaPhi = 0;
  double deltaRR = 0;
  
  TH1F *eta_cells = new TH1F("eta_cells", "", 500, -5.0, 5.0);
  TH1F *phi_cells = new TH1F("phi_cells", "", 500, -5.0, 5.0);
  TH1F *cellEne_ecal = new TH1F("cellEne_ecal", "Cell energy in ECAL", 1000000, 0.0, 0.5*m_beamEnergy);
  TH1F *cellEne_hcal = new TH1F("cellEne_hcal", "Cell energy in HCAL", 1000000, 0.0, 0.5*m_beamEnergy);
 
  const auto ecal_cells = m_ecal_cells.get();
  for (const auto& hit : *ecal_cells) {
    double ene = hit.core().energy;
    sumE_ecal += ene;
    eta = m_ecalBarrelSegmentation->eta( hit.core().cellId );
    phi = m_ecalBarrelSegmentation->phi( hit.core().cellId );
    eta_cells->Fill(eta);
    phi_cells->Fill(phi);
    cellEne_ecal->Fill(ene);
  }
  
  double meanEta = eta_cells->GetMean();
  double meanPhi = phi_cells->GetMean();
  double energy_cut_ecal = 0;
  double energy_cut_hcal = 0;
  double integral = 0;
  double fractionEcut = 1-m_containment;

  //sumE_ecal = cellEne_ecal->Integral(0, cellEne_ecal->GetNbinsX());
  for (int i = 0; i<cellEne_ecal->GetNbinsX()+1;i++) {
    integral += (cellEne_ecal->GetBinLowEdge(i) + cellEne_ecal->GetBinWidth(i)*0.5)*cellEne_ecal->GetBinContent(i);
    if (integral/sumE_ecal > fractionEcut) {
      energy_cut_ecal = cellEne_ecal->GetBinLowEdge(i) + cellEne_ecal->GetBinWidth(i)*0.5;
      break;
    }
  }
  
  if (sumE_ecal>0.01*m_beamEnergy) {
    m_ecut_ecal->Fill(energy_cut_ecal);
  }
 
  for (const auto& hit : *ecal_cells) {    
    double ene = hit.core().energy;    
    if (ene>energy_cut_ecal) {
      sumE_high_ecal += ene;
      eta = m_ecalBarrelSegmentation->eta( hit.core().cellId );
      phi = m_ecalBarrelSegmentation->phi( hit.core().cellId );
      if ( (eta-meanEta)>deltaEta ) {
	deltaEta = eta-meanEta;
      }
      if ( TVector2::Phi_mpi_pi(phi-meanPhi)>deltaPhi ) {
        deltaPhi = TVector2::Phi_mpi_pi(phi-meanPhi);
      }
      double deltaR = sqrt(deltaEta*deltaEta+deltaPhi*deltaPhi);
      if (deltaR>deltaRR) {
        deltaRR = deltaR;
      }
    }
  }
  
  if (sumE_high_ecal>0.01*m_beamEnergy) {
    m_deltaEta_ecal->Fill(deltaEta);      
    m_deltaPhi_ecal->Fill(deltaPhi);
    m_deltaR_ecal->Fill(deltaRR);
  }

 
  const auto hcal_cells = m_hcal_cells.get();
  for (const auto& hit : *hcal_cells) {
    double ene = hit.core().energy;
    sumE_hcal += hit.core().energy;
    cellEne_hcal->Fill(ene);
  }

  integral = 0;
  for (int i = 0; i<cellEne_hcal->GetNbinsX()+1;i++) {
    integral += (cellEne_hcal->GetBinLowEdge(i) + cellEne_hcal->GetBinWidth(i)*0.5)*cellEne_hcal->GetBinContent(i);
    if (integral/sumE_hcal > fractionEcut) {
      energy_cut_hcal = cellEne_hcal->GetBinLowEdge(i) + cellEne_hcal->GetBinWidth(i)*0.5;
      break;
    }
  }
  if (sumE_hcal>0.01*m_beamEnergy) {
    m_ecut_hcal->Fill(energy_cut_hcal);
  }

  deltaEta = 0;
  deltaPhi = 0;
  deltaRR = 0;
  for (const auto& hit : *hcal_cells) {
    double ene = hit.core().energy;
    if (ene>energy_cut_hcal) {
      sumE_high_hcal += ene;
      eta = m_hcalBarrelSegmentation->eta( hit.core().cellId );
      phi = m_hcalBarrelSegmentation->phi( hit.core().cellId );
      if ( (eta-meanEta)>deltaEta ) {
        deltaEta = eta-meanEta;
      }
      if ( TVector2::Phi_mpi_pi(phi-meanPhi)>deltaPhi ) {
        deltaPhi = TVector2::Phi_mpi_pi(phi-meanPhi);
      }
      double deltaR = sqrt(deltaEta*deltaEta+deltaPhi*deltaPhi);
      if (deltaR>deltaRR) {
        deltaRR = deltaR;
      }
      m_cellEnergy_hcal->Fill(ene);
    }
  }
  if (sumE_high_hcal>0.01*m_beamEnergy) {
    m_deltaEta_hcal->Fill(deltaEta);
    m_deltaPhi_hcal->Fill(deltaPhi);
    m_deltaR_hcal->Fill(deltaRR);
  }
  
  debug() << "Total E: in ECAL " << sumE_ecal << " in HCAL " << sumE_hcal << " " << energy_cut_hcal << endmsg;

  m_totalEnergy_ecal->Fill(sumE_ecal);
  m_totalEnergy_hcal->Fill(sumE_hcal);
  m_totalEnergy->Fill(sumE_ecal + sumE_hcal);
  
  m_eneFraction_ecal->Fill(sumE_high_ecal/sumE_ecal);
  m_eneFraction_hcal->Fill(sumE_high_hcal/sumE_hcal);
  
  delete eta_cells;
  delete phi_cells;
  delete cellEne_ecal;
  delete cellEne_hcal;

  return StatusCode::SUCCESS;
}

StatusCode ShowerConeSize::finalize() { return GaudiAlgorithm::finalize(); }
