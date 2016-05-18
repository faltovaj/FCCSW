#include "SimG4Common/MCTruthEventInformation.h"

namespace sim {
MCTruthEventInformation::MCTruthEventInformation() {}

MCTruthEventInformation::~MCTruthEventInformation() {}

void MCTruthEventInformation::Print() const {}

void MCTruthEventInformation::AddParticle(const CLHEP::Hep3Vector& aMom) {
  m_initMomentum = aMom;
}

}
