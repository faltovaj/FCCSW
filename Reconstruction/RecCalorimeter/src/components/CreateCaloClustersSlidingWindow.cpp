#include "CreateCaloClustersSlidingWindow.h"

#include <unordered_map>
#include <utility>
#include <boost/functional/hash.hpp>

// FCCSW
#include "DetInterface/IGeoSvc.h"
#include "DetCommon/DetUtils.h"

// Gaudi
#include "GaudiKernel/PhysicalConstants.h"
//#include "GaudiMath/GaudiMath.h"

// Root
#include "TMath.h"

// datamodel
#include "datamodel/CaloHitCollection.h"
#include "datamodel/CaloClusterCollection.h"

// DD4hep
#include "DD4hep/LCDD.h"

DECLARE_ALGORITHM_FACTORY(CreateCaloClustersSlidingWindow)

CreateCaloClustersSlidingWindow::CreateCaloClustersSlidingWindow(const std::string& name, ISvcLocator* svcLoc)
  : GaudiAlgorithm(name, svcLoc) {
  declareInput("cells", m_cells,"calo/cells");
  declareOutput("clusters", m_clusters,"calo/clusters");
  declareProperty("readoutName", m_readoutName="ECalHitsPhiEta");
  // size of tower
  declareProperty("deltaEtaTower", m_deltaEtaTower=0.01);
  declareProperty("deltaPhiTower", m_deltaPhiTower=2*Gaudi::Units::pi/628);
  // window size (in units of tower)
  declareProperty("nEtaWindow", m_nEtaWindow = 5);
  declareProperty("nPhiWindow", m_nPhiWindow = 5);
  declareProperty("nEtaPosition", m_nEtaPosition = 3);
  declareProperty("nPhiPosition", m_nPhiPosition = 3);
  declareProperty("nEtaDuplicates", m_nEtaDuplicates = 2);
  declareProperty("nPhiDuplicates", m_nPhiDuplicates = 2);
  declareProperty("energyThreshold", m_energyThreshold = 3000);
}

CreateCaloClustersSlidingWindow::~CreateCaloClustersSlidingWindow() {}

StatusCode CreateCaloClustersSlidingWindow::initialize() {
  if (GaudiAlgorithm::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  m_geoSvc = service ("GeoSvc");
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
    return StatusCode::FAILURE;
  }
  // check if readouts exist
  if(m_geoSvc->lcdd()->readouts().find(m_readoutName) == m_geoSvc->lcdd()->readouts().end()) {
    error()<<"Readout <<"<<m_readoutName<<">> does not exist."<<endmsg;
    return StatusCode::FAILURE;
}
  // Get PhiEta segmentation
  m_segmentation = dynamic_cast<DD4hep::DDSegmentation::GridPhiEta*>(m_geoSvc->lcdd()->readout(m_readoutName).segmentation().segmentation());
  if(m_segmentation == nullptr) {
    error()<<"There is no phi-eta segmentation."<<endmsg;
    return StatusCode::FAILURE;
  }
  info() << "CreateCaloClustersSlidingWindow initialized" << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode CreateCaloClustersSlidingWindow::execute() {

  // 1. Get calorimeter towers (calorimeter grid in eta phi, all layers merged)
  prepareTowers();
  buildTowers();

  warning()<<"TOWERS size: "<<m_towers.size()<<endmsg;

  // 2. Find local maxima with sliding window
  std::unordered_map<std::pair<int,int>, float, boost::hash<std::pair<int, int>>> seeds;
  int halfEtaWin =  floor(m_nEtaWindow/2.);
  int halfPhiWin =  floor(m_nPhiWindow/2.);
  // calculate the sum of first m_nEtaWindow bins in eta, for each phi tower
  std::vector<float> sumOverEta (m_nPhiTower, 0);
  for(int iEta = 0; iEta < m_nEtaWindow; iEta++) {
    std::transform (sumOverEta.begin(), sumOverEta.end(), m_towers[iEta].begin(), sumOverEta.begin(), std::plus<float>());
  }

  // loop over all Eta slices starting at the half of the first window
  float sumWindow = 0;
  warning()<<"RANGE: "<<halfEtaWin<<"; "<< m_nEtaTower - halfEtaWin - 1<<endmsg;
  for(int iEta = halfEtaWin; iEta < m_nEtaTower - halfEtaWin; iEta++) {
    info()<<"iEta = "<<iEta<<endmsg;
    // one slice in eta = window, now only sum over window in phi
    // TODO handle corner cases (full phi coverage)
    // sum first window in phi
    for(int iPhiWindow = 0; iPhiWindow <= 2*halfPhiWin; iPhiWindow++) {
      sumWindow += sumOverEta[iPhiWindow];
    }
    for(int iPhi = halfPhiWin; iPhi < m_nPhiTower - halfPhiWin ; iPhi++) {
      if(iEta == 2)
        info()<<"    iPhi = "<<iPhi<<endmsg;
      // if energy is over threshold, test if it is a local maximum
      if(sumWindow > m_energyThreshold) {
        //TODO test local maximum in phi
        //TODO test local maximum in eta
        seeds[std::make_pair(iEta,iPhi)] = sumWindow;
      }
      if(iPhi< m_nPhiTower - halfPhiWin - 1) {
        // finish processing that window, shift window to the next phi tower
        // substract first phi tower in current window
        sumWindow -= sumOverEta[iPhi-halfPhiWin];
        // add next phi tower to the window
        sumWindow += sumOverEta[iPhi+halfPhiWin+1];
      }
    }
    sumWindow = 0;
    // finish processing that slice, shift window to next eta tower
    if(iEta < m_nEtaTower - halfEtaWin - 1) {
      // substract first eta slice in current window
      std::transform (sumOverEta.begin(), sumOverEta.end(), m_towers[iEta-halfEtaWin].begin(), sumOverEta.begin(), std::minus<float>());
      // add next eta slice to the window
      std::transform (sumOverEta.begin(), sumOverEta.end(), m_towers[iEta+halfEtaWin+1].begin(), sumOverEta.begin(), std::plus<float>());
    }
  }
  debug()<<"SEEDS size: "<<seeds.size()<<endmsg;

  //              // TODO add caching of energy (not to repeat all sums)
  //              // TODO add check for local maxima (check closest neighbours)

  // 3. Calculate barycentre position
  std::vector<cluster*> m_preClusters;
  int halfEtaPos =  floor(m_nEtaPosition/2.);
  int halfPhiPos =  floor(m_nPhiPosition/2.);
  float posEta = 0;
  float posPhi = 0;
  float sumEnergy = 0;
  float towEnergy = 0;
  float EMIN = 0.00001;
  for(const auto& seed: seeds) {
    for(int iEta = seed.first.first-halfEtaPos; iEta <= seed.first.first+halfEtaPos; iEta++) {
      for(int iPhi = seed.first.second-halfPhiPos; iPhi <= seed.first.second+halfPhiPos; iPhi++) {
        //int iPhi = seed.first.second;
        towEnergy = m_towers[iEta][iPhi];
        posEta += (iEta - m_nEtaTower/2. )* m_deltaEtaTower  * towEnergy;
	posPhi += (iPhi - m_nPhiTower/2. )* m_deltaPhiTower * towEnergy;
        sumEnergy += towEnergy;
      }
    }
    //If non-zero energy in the cluster, add to pre-clusters (reduced size for pos. calculation -> energy in the core can be zero)
    if (sumEnergy>EMIN) {
      posEta /= sumEnergy;
      posPhi /= sumEnergy;
      cluster* newPreCluster = new cluster();
      newPreCluster->ieta = seed.first.first;
      newPreCluster->iphi = seed.first.second;
      newPreCluster->eta = posEta;
      newPreCluster->phi = posPhi;
      newPreCluster->transEnergy = sumEnergy;
      //debug()<<"PRECLUSTERS: "<<newPreCluster->ieta << " " << newPreCluster->iphi<< " "<<newPreCluster->eta <<" "<<newPreCluster->phi<<" "<<newPreCluster->transEnergy<<endmsg;
      m_preClusters.push_back(newPreCluster);
    }
    posEta = 0;
    posPhi = 0;
    sumEnergy=0;
  }
  debug()<<"PRECLUSTERS size: "<<m_preClusters.size()<<endmsg;

  // 4. Sort the preclusters according to the energy
  std::sort(m_preClusters.begin(),m_preClusters.end(),compareCluster());
  debug()<<"PRECLUSTERS size after sort: "<<m_preClusters.size()<<endmsg;

  // 5. Remove duplicates
  for(auto it1 = m_preClusters.begin(); it1!=m_preClusters.end(); it1++) {
    debug()<<(*it1)->eta << " "<<(*it1)->phi<<" "<< (*it1)->transEnergy <<endmsg;
    for(auto it2 = it1+1; it2!=m_preClusters.end();) {
      debug()<< (*it2)->eta <<" "<<m_preClusters.size()<<endmsg;
      if ( (TMath::Abs((*it1)->eta-(*it2)->eta)<m_nEtaDuplicates*m_deltaEtaTower) || (TMath::Abs((*it1)->phi-(*it2)->phi)<m_nPhiDuplicates*m_deltaPhiTower) ) {
	debug()<<"remove! "<<endmsg;
	delete *it2;  
	it2 = m_preClusters.erase(it2);
      }
      else {
	it2++;
      }
    }
  }
  debug()<<"PRECLUSTERS size after duplicates removal: "<<m_preClusters.size()<<endmsg;


  // 6. Create final clusters

  auto edmClusters = m_clusters.createAndPut();
  for(const auto* clu: m_preClusters) {
    auto edmCluster = edmClusters->create();
    auto& edmClusterCore = edmCluster.core();
    edmClusterCore.position.x = clu->ieta;
    edmClusterCore.position.y = clu->iphi;
    edmClusterCore.position.z = clu->transEnergy;
  }

  return StatusCode::SUCCESS;
}

StatusCode CreateCaloClustersSlidingWindow::finalize() {
  return GaudiAlgorithm::finalize();
}

void CreateCaloClustersSlidingWindow::prepareTowers() {
  auto tubeSizes = det::utils::tubeDimensions(m_segmentation->volumeID(m_cells.get()->at(0).cellId()));
  double maxEta = CLHEP::Hep3Vector(tubeSizes.x(), 0 , tubeSizes.z()).eta();
  m_nEtaTower = 2.*ceil((maxEta-m_deltaEtaTower/2.)/m_deltaEtaTower) + 1;
  m_nPhiTower = 2*ceil((2*Gaudi::Units::pi-m_deltaPhiTower/2.)/m_deltaPhiTower) + 1;

  for(int iEta = 0; iEta < m_nEtaTower; iEta++) {
    m_towers[iEta].assign(m_nPhiTower, 0);
  }
}

void CreateCaloClustersSlidingWindow::buildTowers() {
  // Get the input collection with cells from simulation + digitisation (after calibration and with noise)
  const fcc::CaloHitCollection* cells = m_cells.get();
  debug() << "Input Hit collection size: " << cells->size() << endmsg;
  //Loop through a collection of calorimeter cells and build calo towers (unordered_map)

  for (const auto& ecell : *cells) {
    //Find to which tower the cell belongs (defined by index ieta and iphi)
    std::pair<int, int> ibin;
    findTower(ecell, ibin);
    //save ET
    m_towers[ibin.first][ibin.second] += ecell.core().energy/TMath::CosH(m_segmentation->eta(ecell.core().cellId));
  }
  return;
}

void CreateCaloClustersSlidingWindow::prepareTestTowers() {
  m_nEtaTower = 20;
  m_nPhiTower = 20;

  for(int iEta = 0; iEta < m_nEtaTower; iEta++) {
    m_towers[iEta].assign(m_nPhiTower, 0);
  }
}

void CreateCaloClustersSlidingWindow::buildTestTowers() {
  m_towers[10][15] = 5000;
  m_towers[1][15] = 5000;
  m_towers[7][7] = 3000;
  m_towers[16][6] = 3000;
  m_towers[17][1] = 7000;
  return;
}


void CreateCaloClustersSlidingWindow::findTower(const fcc::CaloHit& aCell, std::pair<int, int>& aTower) {
  //cell coordinates
  float eta = m_segmentation->eta(aCell.core().cellId);
  float phi = m_segmentation->phi(aCell.core().cellId);

  // shift Ids so they start at 0 (segmentation returns IDs that may be from -N to N)
  aTower.first = floor(eta/m_deltaEtaTower) + (m_nEtaTower-1)/2;
  aTower.second = floor(phi/m_deltaPhiTower) + (m_nPhiTower-1)/2;
}
