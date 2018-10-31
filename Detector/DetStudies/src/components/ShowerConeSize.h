#ifndef DETSTUDIES_SHOWERCONESIZE_H
#define DETSTUDIES_SHOWERCONESIZE_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

// FCCSW
#include "DetSegmentation/FCCSWGridPhiEta.h"
#include "FWCore/DataHandle.h"

class IGeoSvc;

// datamodel
namespace fcc {
class PositionedCaloHitCollection;
class CaloHitCollection;
}

namespace dd4hep {
namespace DDSegmentation {
class Segmentation;
}
}

class TH1F;
class TH2F;
class ITHistSvc;
/** @class SumEnergy SumEnergy.h
 *
 *  SumEnergy sum all hit energy
 *
 *  @author Jana Faltova
 */

class ShowerConeSize : public GaudiAlgorithm {
public:
  explicit ShowerConeSize(const std::string&, ISvcLocator*);
  virtual ~ShowerConeSize();
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
  /// Handle for the energy deposits in ECAL
  DataHandle<fcc::CaloHitCollection> m_ecal_cells{"rec/caloHits", Gaudi::DataHandle::Reader, this};
   /// Handle for the energy deposits in HCAL
  DataHandle<fcc::CaloHitCollection> m_hcal_cells{"rec/caloHits", Gaudi::DataHandle::Reader, this};

  // Maximum energy for the axis range
  Gaudi::Property<double> m_beamEnergy{this, "beamEnergy", 100, "Beam energy"};
  // Shower containment
  Gaudi::Property<double> m_containment{this, "containment", 0.95, "containment in ECAL, HCAL, number must be < 1.0"};
  /// Number of layers
  Gaudi::Property<uint> m_numLayers{this, "numLayers", 8, "Number of layers"};
  /// Name of the detector readout
  Gaudi::Property<std::string> m_readoutNameEcal{this, "readoutNameEcal", "", "Name of the detector readout"};
  /// Name of the detector readout                                                                                                                        
  Gaudi::Property<std::string> m_readoutNameHcal{this, "readoutNameHcal", "", "Name of the detector readout"};
  /// Name of the layer/cell field
  Gaudi::Property<std::string> m_layerFieldName{this, "layerFieldName", "", "Identifier of layers"};
  /// ECAL noise per cell 
  Gaudi::Property<std::vector<double>> m_noiseEcalLayer{this,"noiseEcalLayer",{5,10,5,5,6,10,12,15},"Noise of ECAL per layer"};
  /// HCAL noise per cell
  Gaudi::Property<double> m_noiseHcal{this,"noiseHcal",10,"Noise of ECAL per layer"};

  /// PhiEta segmentation of the ecal barrel calorimeter (owned by DD4hep)
  dd4hep::DDSegmentation::FCCSWGridPhiEta* m_ecalBarrelSegmentation;
  /// PhiEta segmentation of the hcal barrel calorimeter (owned by DD4hep)
  dd4hep::DDSegmentation::FCCSWGridPhiEta* m_hcalBarrelSegmentation;

  // Histogram of total energy
  TH1F* m_totalEnergy;
  // Histogram of total energy in ECAL
  TH1F* m_totalEnergy_ecal;
  // Histogram of total energy in HCAL
  TH1F* m_totalEnergy_hcal;

  // deltaEta max in Ecal
  TH1F* m_deltaEta_ecal;
  // deltaPhi max in Ecal
  TH1F* m_deltaPhi_ecal;
  // deltaR max in Ecal
  TH1F* m_deltaR_ecal;
  // deltaEta max in Hcal                                               
  TH1F* m_deltaEta_hcal;
  // deltaPhi max in Hcal                                                  
  TH1F* m_deltaPhi_hcal;  
  // deltaR max in Hcal
  TH1F* m_deltaR_hcal;

  // Cell energy in ECAL 
  TH1F* m_cellEnergy_ecal; 
  // Cell energy in HCAL
  TH1F* m_cellEnergy_hcal; 

  //Energy fraction in cells with energy above threshold
  TH1F* m_eneFraction_ecal;
  //Energy fraction in cells with energy above threshold
  TH1F* m_eneFraction_hcal;
  // 
  TH1F* m_ecut_ecal;
  //
  TH1F* m_ecut_hcal;

};
#endif /* DETSTUDIES_SHOWERCONESIZE_H */
