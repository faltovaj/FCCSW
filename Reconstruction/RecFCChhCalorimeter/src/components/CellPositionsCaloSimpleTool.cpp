#include "CellPositionsCaloSimpleTool.h"

#include "datamodel/CaloHitCollection.h"
#include "datamodel/PositionedCaloHitCollection.h"

DECLARE_COMPONENT(CellPositionsCaloSimpleTool)

CellPositionsCaloSimpleTool::CellPositionsCaloSimpleTool(const std::string& type, const std::string& name,
                                                       const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<ICellPositionsTool>(this);
}

StatusCode CellPositionsCaloSimpleTool::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;
  m_geoSvc = service("GeoSvc");
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry service." << endmsg;
    return StatusCode::FAILURE;
  }
  // get PhiEta segmentation
  m_segmentation = dynamic_cast<dd4hep::DDSegmentation::FCCSWGridPhiEta*>(
      m_geoSvc->lcdd()->readout(m_readoutName.value()).segmentation().segmentation());
  if (m_segmentation == nullptr) {
    error() << "There is no phi-eta segmentation!!!!" << endmsg;
    return StatusCode::FAILURE;
  }
  // Take readout bitfield decoder from GeoSvc
  m_decoder = m_geoSvc->lcdd()->readout(m_readoutName.value()).idSpec().decoder();
  m_volman = m_geoSvc->lcdd()->volumeManager();
  // check if decoder contains m_activeFieldName ('layer')
  std::vector<std::string> fields;
  for (uint itField = 0; itField < m_decoder->size(); itField++) {
    fields.push_back((*m_decoder)[itField].name());
  }
  auto iter = std::find(fields.begin(), fields.end(), m_activeFieldName);
  if (iter == fields.end()) {
    error() << "Readout does not contain field: " << m_activeFieldName << endmsg;
  }
  return sc;
}

void CellPositionsCaloSimpleTool::getPositions(const fcc::CaloHitCollection& aCells,
                                              fcc::PositionedCaloHitCollection& outputColl) {
  debug() << "Input collection size : " << aCells.size() << endmsg;
  // Loop through cell collection
  for (const auto& cell : aCells) {
    auto outPos = CellPositionsCaloSimpleTool::xyzPosition(cell.core().cellId);

    auto edmPos = fcc::Point();
    edmPos.x = outPos.x() / dd4hep::mm;
    edmPos.y = outPos.y() / dd4hep::mm;
    edmPos.z = outPos.z() / dd4hep::mm;
    auto positionedHit = outputColl.create(edmPos, cell.core());
    // Debug information about cell position
    debug() << "Cell energy (GeV) : " << cell.core().energy << "\tcellID " << cell.core().cellId << endmsg;
    debug() << "Position of cell (mm) : \t" << outPos.x() / dd4hep::mm << "\t" << outPos.y() / dd4hep::mm << "\t"
            << outPos.z() / dd4hep::mm << endmsg;
  }
  debug() << "Output positions collection size: " << outputColl.size() << endmsg;
}

dd4hep::Position CellPositionsCaloSimpleTool::xyzPosition(const uint64_t& aCellId) const {
  double radius;
  dd4hep::Position outPos;
  dd4hep::DDSegmentation::CellID volumeId = aCellId;
  // retrieve layer Id, eta and phi positions of the cell
  auto inSeg = m_segmentation->position(aCellId);
  double eta = m_segmentation->eta(aCellId);
  double phi = m_segmentation->phi(aCellId);
  int layerId = m_decoder->get(aCellId, m_activeFieldName);
  debug() << "Segmentation: eta : \t" << eta << " phi : \t" << phi << " layer : \t" << layerId << endmsg;
  radius = m_rminFirstLayer + m_layerThickness * (0.5 + layerId); 
  debug() << "Radius : " << radius << endmsg;
  outPos.SetCoordinates(cos(phi) * radius, sin(phi) * radius, sinh(eta) * radius);

  return outPos;
}

int CellPositionsCaloSimpleTool::layerId(const uint64_t& aCellId) {
  int layer;
dd4hep::DDSegmentation::CellID cID = aCellId;
 layer = m_decoder->get(cID, "layer");
  return layer;
}

StatusCode CellPositionsCaloSimpleTool::finalize() { return GaudiTool::finalize(); }
