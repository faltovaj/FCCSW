#include "SumEnergy.h"
// FCCSW
#include "DetInterface/IGeoSvc.h"
// datamodel
#include "datamodel/CaloHitCollection.h"   

#include "CLHEP/Vector/ThreeVector.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TH1F.h"

// DD4hep
#include "DD4hep/Detector.h"

#include <math.h>
#include <iostream>

DECLARE_ALGORITHM_FACTORY(SumEnergy)

SumEnergy::SumEnergy(const std::string& aName, ISvcLocator* aSvcLoc)
    : GaudiAlgorithm(aName, aSvcLoc),
      m_histSvc("THistSvc", "SumEnergy"),
  m_geoSvc("GeoSvc", "SumEnergy") {
  declareProperty("cells", m_cells, "calorimeter cells (input)");
}
SumEnergy::~SumEnergy() {}

StatusCode SumEnergy::initialize() {
  if (GaudiAlgorithm::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  // create histograms
  m_totalEnergy = new TH1F("totalEnergy", "Total energy (active + passive, visible + invisible)", 500, 0.0, 1.2 * m_beamEnergy);
  if (m_histSvc->regHist("/rec/totalEnergy", m_totalEnergy).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_visibleEnergy = new TH1F("visibleEnergy", "Total visible energy (active + passive)", 500, 0.0, 1.2 * m_beamEnergy);
  if (m_histSvc->regHist("/rec/visibleEnergy", m_visibleEnergy).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_totalEnergy_ecal10 = new TH1F("totalEnergy_ecal10", "Total energy (active + passive, visible + invisible)", 500, 0.0, 1.2 * m_beamEnergy);
  if (m_histSvc->regHist("/rec/totalEnergy_ecal10", m_totalEnergy_ecal10).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_visibleEnergy_ecal10 = new TH1F("visibleEnergy_ecal10", "Total visible energy (active + passive)", 500, 0.0, 1.2 * m_beamEnergy);
  if (m_histSvc->regHist("/rec/visibleEnergy_ecal10", m_visibleEnergy_ecal10).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_eOverPi = new TH1F("eOverPi", "e/pi ratio", 200, 0.5, 2.5);
  if (m_histSvc->regHist("/rec/eOverPi", m_eOverPi).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_eOverPi_ecal10 = new TH1F("eOverPi_ecal10", "e/pi ratio", 200, 0.5, 2.5);
  if (m_histSvc->regHist("/rec/eOverPi_ecal10", m_eOverPi_ecal10).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_eOverPi_ecal20 = new TH1F("eOverPi_ecal20", "e/pi ratio", 200, 0.5, 2.5);
  if (m_histSvc->regHist("/rec/eOverPi_ecal20", m_eOverPi_ecal20).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }


  
  return StatusCode::SUCCESS;
}

StatusCode SumEnergy::execute() {
 
  double sumEvis = 0.;
  double sumEtotal = 0.;
  const auto cells = m_cells.get();
  for (const auto& hit : *cells) {
    sumEvis += hit.core().energy;
    sumEtotal += hit.core().time;
  }
  
  debug() << "Total E " << sumEtotal << " vis " << sumEvis << endmsg;

  m_totalEnergy->Fill(sumEtotal);
  m_visibleEnergy->Fill(sumEvis);
  if (sumEvis>0) {
    m_eOverPi->Fill(sumEtotal/sumEvis);
    if (sumEvis>0.1*m_beamEnergy) {
      m_eOverPi_ecal10->Fill(sumEtotal/sumEvis);
      m_totalEnergy_ecal10->Fill(sumEtotal);
      m_visibleEnergy_ecal10->Fill(sumEvis);
      if (sumEvis>0.2*m_beamEnergy) {
	m_eOverPi_ecal20->Fill(sumEtotal/sumEvis);
      }
    }
  }  

  return StatusCode::SUCCESS;
}

StatusCode SumEnergy::finalize() { return GaudiAlgorithm::finalize(); }
