// DD4hep includes
#include "DD4hep/DetFactoryHelper.h"

// Gaudi
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

// FCCSW includes
// #include "DetExtensions/DetCylinderVolume.h"

using DD4hep::Geometry::Volume;
using DD4hep::Geometry::DetElement;
using DD4hep::XML::Dimension;
using DD4hep::Geometry::PlacedVolume;

namespace det {

static DD4hep::Geometry::Ref_t createECal (DD4hep::Geometry::LCDD& lcdd,xml_h xmlElement,
				DD4hep::Geometry::SensitiveDetector sensDet) 
{
  ServiceHandle<IMessageSvc> msgSvc("MessageSvc", "ECalConstruction");
  MsgStream lLog(&(*msgSvc), "ECalConstruction");

  xml_det_t xmlDet = xmlElement;
  std::string detName = xmlDet.nameStr();
  //Make DetElement
  DetElement eCal(detName, xmlDet.id());

  // Make volume that envelopes the whole barrel; set material to air
  Dimension dimensions(xmlDet.dimensions());
  DD4hep::Geometry::Tube envelopeShape(dimensions.rmin(), dimensions.rmax(), dimensions.dz());
  Volume envelopeVolume(detName, envelopeShape, lcdd.air());
  // Invisibility seems to be broken in visualisation tags, have to hardcode that
  // envelopeVolume.setVisAttributes(lcdd, dimensions.visStr());
  envelopeVolume.setVisAttributes(lcdd.invisible());
  
  xml_comp_t cryostat = xmlElement.child("cryostat");
  Dimension cryo_dims(cryostat.dimensions());
  double cryo_thickness=cryo_dims.thickness();
  
  xml_comp_t calo = xmlElement.child("calorimeter");
  Dimension calo_dims(calo.dimensions());
  std::string calo_name=calo.nameStr();
  double calo_id=calo.id();
  
  xml_comp_t active = calo.child("active_layers");
  std::string active_mat=active.materialStr();
  double active_tck=active.thickness();
  
  xml_comp_t passive = calo.child("passive_layers");
  std::string passive_mat=passive.materialStr();
  double passive_tck=passive.thickness();  

  // Step 1 : cryostat
  
  DetElement cryo(cryostat.nameStr(), 0);
  DD4hep::Geometry::Tube cryoShape(cryo_dims.rmin() , cryo_dims.rmax(), cryo_dims.dz());
  lLog << MSG::INFO << "ECAL Building cryostat from " << cryo_dims.rmin() << " to " << cryo_dims.rmax() << endmsg;
  Volume cryoVol(cryostat.nameStr(), cryoShape, lcdd.material(cryostat.materialStr()));
  PlacedVolume placedCryo = envelopeVolume.placeVolume(cryoVol);
  placedCryo.addPhysVolID("ECAL_Cryo", cryostat.id());
  cryo.setPlacement(placedCryo);

  // Step 2 : fill cryostat with active medium

  DetElement calo_bath(active_mat, 0);
  DD4hep::Geometry::Tube bathShape(cryo_dims.rmin()+cryo_thickness , cryo_dims.rmax()-cryo_thickness, cryo_dims.dz()-cryo_thickness);
  lLog << MSG::INFO << "ECAL: Filling cryostat with active medium from " << cryo_dims.rmin()+cryo_thickness << " to " << cryo_dims.rmax()-cryo_thickness << endmsg;
  Volume bathVol(active_mat, bathShape, lcdd.material(active_mat));
  PlacedVolume placedBath = cryoVol.placeVolume(bathVol);
  placedBath.addPhysVolID("active", 0);
  calo_bath.setPlacement(placedBath);
  
  // Step 3 : create the actual calorimeter

  int n_samples_r= (calo_dims.rmax()-  calo_dims.rmin() - passive_tck)/(passive_tck+active_tck);
  lLog << MSG::INFO <<"++++++++++++++++++++++++++ nSamplings in r"<<n_samples_r<<endmsg;

  //double halfz_cell = calo_dims.dz();
  double halfz_cell = calo_dims.width();  

  lLog << MSG::INFO << "Cell size in z" << 2*halfz_cell << endmsg;

  int n_samples_z = calo_dims.dz()/halfz_cell;
  lLog << MSG::INFO <<"++++++++++++++++++++++++++ nSamplings in z "<<n_samples_z<<endmsg;

  //Real dimensions defined by the size of the cells/layers  
  double calo_tck=n_samples_r*(active_tck+passive_tck)+passive_tck;
  double calo_halfz=n_samples_z*halfz_cell;

  //JANA: commented out
  //DetElement caloDet(calo_name, calo_id);

  if ( (calo_dims.rmin()<(cryo_dims.rmin()+cryo_thickness)) || 
       ((calo_dims.rmin()+calo_tck)>(cryo_dims.rmax()-cryo_thickness)) ) {
    lLog << MSG::WARNING <<"Overlay in ECal construction!!!!!" << endmsg;
    lLog << MSG::WARNING <<" Cryo boundaries "<< cryo_dims.rmin()+cryo_thickness << " , " << cryo_dims.rmax()-cryo_thickness <<endmsg;
    lLog << MSG::WARNING <<" Calo volume from " << calo_dims.rmin() << " to " << calo_dims.rmin()+calo_tck <<endmsg;
  }

  DD4hep::Geometry::Tube caloShape(calo_dims.rmin(), calo_dims.rmin()+calo_tck, halfz_cell);
  lLog << MSG::INFO << "ECAL: Building the actual calorimeter from " << calo_dims.rmin() << " to " <<   calo_dims.rmin()+calo_tck << endmsg;
  Volume caloVol(passive_mat, caloShape, lcdd.material(passive_mat));
  //JANA:commented
  //PlacedVolume placedCalo = bathVol.placeVolume(caloVol);
  //placedCalo.addPhysVolID("EM_barrel", calo_id);
  //caloDet.setPlacement(placedCalo);
  //JANA: How to make the detector parts visible?
  //xml_comp_t xCaloVol = xmlElement.child("caloVol");
  //caloVol.setVisAttributes(lcdd,xCaloVol.visStr());
  
  // set the sensitive detector type to the DD4hep calorimeter
  sensDet.setType("Geant4Calorimeter");

  // loop on the sensitive layers
  
  for (int i=0;i<n_samples_r;i++)
  {
  	double layer_r=calo_dims.rmin()+passive_tck+i*(passive_tck+active_tck);
  	DetElement caloLayer(active_mat+"_sensitive", i+1);
  	DD4hep::Geometry::Tube layerShape(layer_r , layer_r+active_tck, calo_halfz);
	// lLog << MSG::DEBUG << "ECAL senst. layers :  #" << i << " from " << layer_r << " to " <<  layer_r+active_tck << endmsg;
  	Volume layerVol(active_mat, layerShape, lcdd.material(active_mat));
  	PlacedVolume placedLayer = caloVol.placeVolume(layerVol);
	placedLayer.addPhysVolID("r_layer", i+1);
  	caloLayer.setPlacement(placedLayer);
	layerVol.setSensitiveDetector(sensDet);
  }
  
  //n_samples_z = 1;
  for (int i=0;i<n_samples_z;i++)
    {
      double zOffset = -calo_halfz+(2*i+1)*halfz_cell;
      lLog << MSG::INFO <<"=== zOffset " << zOffset << endmsg;
      DD4hep::Geometry::Position offset(0, 0, zOffset);
      DetElement caloZDet("calo_cells_inz", i+1);
      PlacedVolume placedCaloZVolume = bathVol.placeVolume(caloVol, offset);
      placedCaloZVolume.addPhysVolID("z_layer", i+1);
      caloZDet.setPlacement(placedCaloZVolume);
    }
 
 
  //Place envelope (or barrel) volume
  Volume motherVol = lcdd.pickMotherVolume(eCal);
  PlacedVolume placedECal = motherVol.placeVolume(envelopeVolume);
  placedECal.addPhysVolID("system", eCal.id());
  eCal.setPlacement(placedECal);
 
  return eCal;

}
} // namespace det

DECLARE_DETELEMENT(EmCaloBarrel, det::createECal)

