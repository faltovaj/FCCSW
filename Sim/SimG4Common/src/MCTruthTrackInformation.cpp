#include "SimG4Common/MCTruthTrackInformation.h"

namespace sim {
MCTruthTrackInformation::MCTruthTrackInformation() {}

MCTruthTrackInformation::~MCTruthTrackInformation() {}

void MCTruthTrackInformation::Print() const {}

void MCTruthTrackInformation::SetInitMomentum(const CLHEP::Hep3Vector& aMom) {
  m_initMomentum = aMom;
}
const CLHEP::Hep3Vector& MCTruthTrackInformation::GetInitMomentum() const {
  return m_initMomentum;
}
void MCTruthTrackInformation::SetInitVertexPosition(const CLHEP::Hep3Vector& aPos) {
  m_initVertexPosition = aPos;
}
const CLHEP::Hep3Vector& MCTruthTrackInformation::GetInitVertexPosition() const {
  return m_initVertexPosition;
}
void MCTruthTrackInformation::SetEndVertexPosition(const CLHEP::Hep3Vector& aPos) {
  m_endVertexPosition = aPos;
}
const CLHEP::Hep3Vector& MCTruthTrackInformation::GetEndVertexPosition() const {
  return m_endVertexPosition;
}
}
