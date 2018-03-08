#ifndef RECCALORIMETER_PREPAREPILEUP_H
#define RECCALORIMETER_PREPAREPILEUP_H

// FCCSW
#include "FWCore/DataHandle.h"
#include "RecInterface/ICalorimeterTool.h"
#include "DetSegmentation/FCCSWGridPhiEta.h"
#include "RecInterface/ICellPositionsTool.h"
class IGeoSvc;

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// EDM
#include "datamodel/CaloHit.h"
#include "datamodel/CaloHitCollection.h"

// CLHEP         
#include "CLHEP/Vector/ThreeVector.h"

class TH2F;
class TH1F;
class ITHistSvc;

/** @class PreparePileup
 *
 *  Algorithm for calculating pileup noise per cell.
 *  Tube geometry with PhiEta segmentation expected.
 * 
 *  TODO: extend to calculate noise per cluster.
 *
 *  Flow of the program:
 *  1/ Create a map of empty cells (m_geoTool)
 *  2/ Merge pileup signal events with empty cells
 *  3/ Fill histograms - energy, |eta| in TH2 histogram
 *
 *  Tools called:
 *    - TubeLayerPhiEtaCaloTool
 *
 *  @author Jana Faltova, Anna Zaborowska
 *  @date   2018-01
 *
 */

class PreparePileup : public GaudiAlgorithm {

public:
  PreparePileup(const std::string& name, ISvcLocator* svcLoc);

  StatusCode initialize();

  StatusCode execute();

  StatusCode finalize();

  dd4hep::DDSegmentation::BitField64* m_decoder;
  
private:
  /// Handle for geometry tool (used to prepare map of all existing cellIDs for the system)
  ToolHandle<ICalorimeterTool> m_geoTool{"TubeLayerPhiEtaCaloTool", this};
  /// Handle for tool to get positions in ECal Barrel
  ToolHandle<ICellPositionsTool> m_cellPositionsTool{"CellPositionsTool", this};
  /// Handle for calo hits (input collection)
  DataHandle<fcc::CaloHitCollection> m_hits{"hits", Gaudi::DataHandle::Reader, this};

  /// Map of cell IDs (corresponding to DD4hep IDs) and energy
  std::unordered_map<uint64_t, double> m_cellsMap;
  /// Sum of energy in each cell per file
  std::unordered_map<uint64_t, double> m_sumEnergyCellsMap;

  /// Pointer to the interface of histogram service
  SmartIF<ITHistSvc> m_histSvc;
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  /// 2D histogram with abs(eta) on x-axis and energy per cell per event on y-axis
  std::vector<TH2F*> m_energyVsAbsEta;
  /// 2D histogram with abs(eta) on x-axis and energy per cell per file on y-axis
  std::vector<TH2F*> m_energyAllEventsVsAbsEta;

  /// Maximum energy in the m_energyVsAbsEta histogram, in GeV 
  Gaudi::Property<uint> m_maxEnergy{this, "maxEnergy", 20, "Maximum energy in the pile-up plot"};
  /// Maximum in the eta/z axis, used to identiy if x axis is eta or z 
  Gaudi::Property<uint> m_xAxisMax{this, "xAxisMax", 6, "Maximum of x axis"};
  /// Name of the pileup histograms
  Gaudi::Property<std::string> m_histogramName{this, "histogramName", "energyVsAbsEta", "Name of 2D histogram"};
  /// Name of the layer field
  Gaudi::Property<std::string> m_layerFieldName{this, "layerFieldName", "layer", "Name of layer"};
  /// Number of layers
  Gaudi::Property<uint> m_numLayers{this, "numLayers", 8, "Number of layers"};
   /// Name of the detector readout
  Gaudi::Property<std::string> m_readoutName{this, "readoutName", "", "Name of the readout"};
  /// PhiEta segmentation (owned by DD4hep)
  dd4hep::DDSegmentation::FCCSWGridPhiEta* m_segmentation;
};

#endif /* RECCALORIMETER_PREPAREPILEUP_H */
