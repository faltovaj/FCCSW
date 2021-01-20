#ifndef RECCALORIMETER_CELLPOSITIONSCALOSIMPLETOOL_H
#define RECCALORIMETER_CELLPOSITIONSCALOSIMPLETOOL_H

// GAUDI
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ServiceHandle.h"

// FCCSW
#include "DetCommon/DetUtils.h"
#include "DetInterface/IGeoSvc.h"
#include "DetSegmentation/FCCSWGridPhiEta.h"
#include "FWCore/DataHandle.h"
#include "RecInterface/ICellPositionsTool.h"

// DD4hep
#include "DD4hep/Readout.h"
#include "DD4hep/Volumes.h"
#include "DDSegmentation/Segmentation.h"
#include "TGeoManager.h"

class IGeoSvc;
namespace DD4hep {
namespace DDSegmentation {
class Segmentation;
}
}

/** @class CellPositionsCaloSimpleTool Reconstruction/RecFCChhCalorimeter/src/components/CellPositionsCaloSimpleTool.h
 *   CellPositionsCaloSimpleTool.h
 *
 *  Tool to determine each Calorimeter cell position.
 *
 *  For the FCChh Calo Simple, determined from the placed volumes and the FCCSW eta-phi segmentation. 
 * 
 *  @author Coralie Neubueser
*/

class CellPositionsCaloSimpleTool : public GaudiTool, virtual public ICellPositionsTool {

public:
  CellPositionsCaloSimpleTool(const std::string& type, const std::string& name, const IInterface* parent);
  ~CellPositionsCaloSimpleTool() = default;

  virtual StatusCode initialize() final;

  virtual StatusCode finalize() final;

  virtual void getPositions(const fcc::CaloHitCollection& aCells, fcc::PositionedCaloHitCollection& outputColl) final;

  virtual dd4hep::Position xyzPosition(const uint64_t& aCellId) const final;

  virtual int layerId(const uint64_t& aCellId) final;

private:
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  /// Name of the electromagnetic calorimeter readout
  Gaudi::Property<std::string> m_readoutName{this, "readoutName", "ECalHitsPhiEta", "name of the readout"};
  /// Radius of the first layer (rmin value)
  Gaudi::Property<double> m_rminFirstLayer{this, "rminFirstLayer", 195.0, "rmin position of the first layer"};
  /// Thickness of the layers of the calorimeter 
  Gaudi::Property<double> m_layerThickness{this, "layerThickness", 0.6, "thickness of the calorimeter layer"};
  /// Name of active layers 
  Gaudi::Property<std::string> m_activeFieldName{this, "activeFieldName", "active_layer", "Name of active layers for sampling calorimeter"};
  /// Eta-phi segmentation
  dd4hep::DDSegmentation::FCCSWGridPhiEta* m_segmentation;
  /// Cellid decoder
  dd4hep::DDSegmentation::BitFieldCoder* m_decoder;
  /// Volume manager
  dd4hep::VolumeManager m_volman;
};
#endif /* RECCALORIMETER_CELLPOSITIONSCALOSIMPLETOOL_H */
