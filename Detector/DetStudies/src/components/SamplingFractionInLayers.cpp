#include "SamplingFractionInLayers.h"

// FCCSW
#include "DetInterface/IGeoSvc.h"

// datamodel
//#include "datamodel/PositionedCaloHitCollection.h"
#include "datamodel/CaloHitCollection.h"   

#include "CLHEP/Vector/ThreeVector.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TH1F.h"
#include "TVector2.h"

// DD4hep
#include "DD4hep/Detector.h"
#include "DD4hep/Readout.h"

#include <math.h>
#include <iostream> 

DECLARE_ALGORITHM_FACTORY(SamplingFractionInLayers)

SamplingFractionInLayers::SamplingFractionInLayers(const std::string& aName, ISvcLocator* aSvcLoc)
    : GaudiAlgorithm(aName, aSvcLoc),
      m_histSvc("THistSvc", "SamplingFractionInLayers"),
      m_geoSvc("GeoSvc", "SamplingFractionInLayers"),
      m_totalEnergy(nullptr),
      m_totalActiveEnergy(nullptr),
      m_sf(nullptr) {
  declareProperty("deposits", m_deposits, "Energy deposits in sampling calorimeter (input)");
  declareProperty("hcalCells", m_hcalCells, "hcal cells (input)");
}
SamplingFractionInLayers::~SamplingFractionInLayers() {}

StatusCode SamplingFractionInLayers::initialize() {
  if (GaudiAlgorithm::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  // check if readouts exist
  if (m_geoSvc->lcdd()->readouts().find(m_readoutName) == m_geoSvc->lcdd()->readouts().end()) {
    error() << "Readout <<" << m_readoutName << ">> does not exist." << endmsg;
    return StatusCode::FAILURE;
  }
  // create histograms
  for (uint i = 0; i < m_numLayers; i++) {
    m_totalEnLayers.push_back(new TH1F(("ecal_totalEnergy_layer" + std::to_string(i)).c_str(),
                                       ("Total deposited energy in layer " + std::to_string(i)).c_str(), 1000, 0,
                                       1.2 * m_beamEnergy));
    if (m_histSvc->regHist("/rec/ecal_total_layer" + std::to_string(i), m_totalEnLayers.back()).isFailure()) {
      error() << "Couldn't register histogram" << endmsg;
      return StatusCode::FAILURE;
    }
    m_activeEnLayers.push_back(new TH1F(("ecal_activeEnergy_layer" + std::to_string(i)).c_str(),
                                        ("Deposited energy in active material, in layer " + std::to_string(i)).c_str(),
                                        1000, 0, 1.2 * m_beamEnergy));
    if (m_histSvc->regHist("/rec/ecal_active_layer" + std::to_string(i), m_activeEnLayers.back()).isFailure()) {
      error() << "Couldn't register histogram" << endmsg;
      return StatusCode::FAILURE;
    }
    m_sfLayers.push_back(new TH1F(("ecal_sf_layer" + std::to_string(i)).c_str(),
                                  ("SF for layer " + std::to_string(i)).c_str(), 1000, 0, 1));
    if (m_histSvc->regHist("/rec/ecal_sf_layer" + std::to_string(i), m_sfLayers.back()).isFailure()) {
      error() << "Couldn't register histogram" << endmsg;
      return StatusCode::FAILURE;
    }
  }
  m_totalEnergy = new TH1F("ecal_totalEnergy", "Total deposited energy", 1000, 0, 1.2 * m_beamEnergy);
  if (m_histSvc->regHist("/rec/ecal_total", m_totalEnergy).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_totalActiveEnergy = new TH1F("ecal_active", "Deposited energy in active material", 1000, 0, 1.2 * m_beamEnergy);
  if (m_histSvc->regHist("/rec/ecal_active", m_totalActiveEnergy).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_sf = new TH1F("ecal_sf", "Sampling fraction", 1000, 0, 1);
  if (m_histSvc->regHist("/rec/ecal_sf", m_sf).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_eOverPi_ecal = new TH1F("ecal_eOverPi", "e/pi in ECAL", 100, 0, 4.0);
  if (m_histSvc->regHist("/rec/ecal_eOverPi", m_eOverPi_ecal).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_eOverPi_ecal10 = new TH1F("ecal_eOverPi10", "e/pi in ECAL, >10% of beam energy in ECAL", 100, 0, 4.0);
  if (m_histSvc->regHist("/rec/ecal_eOverPi10", m_eOverPi_ecal10).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_eOverPi_ecal20 = new TH1F("ecal_eOverPi20", "e/pi in ECAL, >20% of beam energy in ECAL", 100, 0, 4.0);
  if (m_histSvc->regHist("/rec/ecal_eOverPi20", m_eOverPi_ecal20).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_totalEnergy_hcal = new TH1F("hcal_totalEnergy", "Total deposited energy", 1000, 0, 2.0 * m_beamEnergy);
  if (m_histSvc->regHist("/rec/hcal_total", m_totalEnergy_hcal).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode SamplingFractionInLayers::execute() {
  auto decoder = m_geoSvc->lcdd()->readout(m_readoutName).idSpec().decoder();
  double sumE = 0.;
  double sumE_ecal = 0.;
  std::vector<double> sumElayers;
  double sumEactive = 0.;
  std::vector<double> sumEactiveLayers;
  sumElayers.assign(m_numLayers, 0);
  sumEactiveLayers.assign(m_numLayers, 0);

  const auto deposits = m_deposits.get();
  for (const auto& hit : *deposits) {
    sumE_ecal += hit.core().energy;
    decoder->setValue(hit.core().cellId);
    sumElayers[(*decoder)[m_layerFieldName]] += hit.core().energy;
    // check if energy was deposited in the calorimeter (active/passive material)
    if ((*decoder)[m_layerFieldName] >= m_firstLayerId) {
      sumE += hit.core().energy;
      // active material of calorimeter
      if ((*decoder)[m_activeFieldName] == m_activeFieldValue) {
        sumEactive += hit.core().energy;
        sumEactiveLayers[(*decoder)[m_layerFieldName]] += hit.core().energy;
      }
    }
  }


  // HCAL cells calibrated to hadronic scale!!! (Coralie)
  double sumE_hcal = 0;
  const auto hcalCells = m_hcalCells.get();
  for (const auto& hit : *hcalCells) {
    sumE_hcal += hit.core().energy;
  }
  // e/pi in ECAL - corrected for leakage into HCAL
  // sumE - EM scale
  double eOverPi_ecal = ( m_beamEnergy - sumE_hcal ) / sumE_ecal;
  
  // Fill histograms
  m_totalEnergy->Fill(sumE);
  m_totalActiveEnergy->Fill(sumEactive);
  m_totalEnergy_hcal->Fill(sumE_hcal);
  if (sumE > 0) {
    m_eOverPi_ecal->Fill(eOverPi_ecal);
    m_sf->Fill(sumEactive / sumE);
  }
  if (sumE_ecal > 0.1 * m_beamEnergy) {
    std::cout << " ecal e/pi " << eOverPi_ecal << " hcal E (hadronic scale) " << sumE_hcal << " ecal E " << sumE << std::endl;
    m_eOverPi_ecal10->Fill(eOverPi_ecal);
  }
  if (sumE_ecal > 0.2 * m_beamEnergy) {
    m_eOverPi_ecal20->Fill(eOverPi_ecal);
  }
  for (uint i = 0; i < m_numLayers; i++) {
    m_totalEnLayers[i]->Fill(sumElayers[i]);
    m_activeEnLayers[i]->Fill(sumEactiveLayers[i]);
    if (i < m_firstLayerId) {
      debug() << "total energy deposited outside the calorimeter detector = " << sumElayers[i] << endmsg;
    } else {
      debug() << "total energy in layer " << i << " = " << sumElayers[i] << " active = " << sumEactiveLayers[i]
              << endmsg;
    }
    if (sumElayers[i] > 0) {
      m_sfLayers[i]->Fill(sumEactiveLayers[i] / sumElayers[i]);
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode SamplingFractionInLayers::finalize() { return GaudiAlgorithm::finalize(); }
