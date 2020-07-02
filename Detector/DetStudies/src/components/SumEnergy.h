#ifndef DETSTUDIES_SUMENERGY_H
#define DETSTUDIES_SUMENERGY_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

// FCCSW
#include "FWCore/DataHandle.h"
class IGeoSvc;

// datamodel
namespace fcc {
class PositionedCaloHitCollection;
class CaloHitCollection;
}

class TH1F;
class ITHistSvc;
/** @class SumEnergy SumEnergy.h
 *
 *  SumEnergy sum all hit energy
 *
 *  @author Jana Faltova
 */

class SumEnergy : public GaudiAlgorithm {
public:
  explicit SumEnergy(const std::string&, ISvcLocator*);
  virtual ~SumEnergy();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize() final;
  /**  Fills the histograms.
   *   @return status code
   */
  virtual StatusCode execute() final;
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize() final;

private:
  /// Pointer to the interface of histogram service
  ServiceHandle<ITHistSvc> m_histSvc;
  /// Pointer to the geometry service
  ServiceHandle<IGeoSvc> m_geoSvc;
  /// Handle for the energy deposits
  DataHandle<fcc::CaloHitCollection> m_cells{"rec/caloHits", Gaudi::DataHandle::Reader, this};
 
  // Maximum energy for the axis range
  Gaudi::Property<double> m_beamEnergy{this, "beamEnergy", 500, "Beam energy"};

  // Histogram of total energy (active + passive, visible + invisible)
  TH1F* m_totalEnergy;
  // Histogram of total visible energy (active + passive, visible + invisible)
  TH1F* m_visibleEnergy;
  // Histogram of e/pi
  TH1F* m_eOverPi;
  // Histogram of e/pi, min. 10% of beam energy in ECAL
  TH1F* m_eOverPi_ecal10;
  // Histogram of total energy (active + passive, visible + invisible), min. 10% of beam energy in ECAL
  TH1F* m_totalEnergy_ecal10;
  // Histogram of total visible energy (active + passive, visible + invisible), min. 10% of beam energy in ECAL
  TH1F* m_visibleEnergy_ecal10;
  // Histogram of e/pi, min. 20% of beam energy in ECAL
  TH1F* m_eOverPi_ecal20;  

};
#endif /* DETSTUDIES_SUMENERGY_H */
