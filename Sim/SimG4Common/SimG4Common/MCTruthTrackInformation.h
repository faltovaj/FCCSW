#ifndef SIMG4COMMON_MCTRUTHTRACKINFORMATION_H
#define SIMG4COMMON_MCTRUTHTRACKINFORMATION_H

// datamodel
//#include "datamodel/MCParticle.h"
//#include "datamodel/Particle.h"

// Geant4
#include "G4VUserTrackInformation.hh"

// CLHEP
#include "CLHEP/Vector/ThreeVector.h"

/** @class MCTruthTrackInformation SimG4Common/SimG4Common/MCTruthTrackInformation.h MCTruthTrackInformation.h
 *
 *  Describes the information that can be assosiated with a G4Track, tracing MC truth particles
 *  
 *  @author Jana Faltova
 */

namespace sim {
class MCTruthTrackInformation: public G4VUserTrackInformation {
public:
  /** A constructor.
   *  @param[in] G4Track& aTrack
   */
  MCTruthTrackInformation();
  /// A destructor
  virtual ~MCTruthTrackInformation();
  /// A printing method
  virtual void Print() const final;
 
  /** Setter of the init-track momentum
   *  @param[in] aMom Particle momentum.
   */
  void SetInitMomentum(const CLHEP::Hep3Vector& aMom);
  /** Getter of the init-track momentum
   *  @returns Init momentum.
   */
  const CLHEP::Hep3Vector& GetInitMomentum() const;
  /** Setter of the init vertex
   *  @param[in] aPos init Vertex position.
   */
  void SetInitVertexPosition(const CLHEP::Hep3Vector& aPos);
  /** Getter of the init vertex positon
   *  @returns init Vertex position.
   */
  const CLHEP::Hep3Vector& GetInitVertexPosition() const;
  /** Setter of the end vertex
   *  @param[in] aPos end Vertex position.
   */
  void SetEndVertexPosition(const CLHEP::Hep3Vector& aPos);
  /** Getter of the end vertex positon
   *  @returns end Vertex position.
   */
  const CLHEP::Hep3Vector& GetEndVertexPosition() const;

 
private:
  /// EDM MC particle
  CLHEP::Hep3Vector m_initMomentum;
  /// Particle vertex position saved at the end of tracking (filled for fast-sim)
  CLHEP::Hep3Vector m_initVertexPosition;
  CLHEP::Hep3Vector m_endVertexPosition;
};
}

#endif /* SIMG4COMMON_MCTRUTHUSERTRACKINFORMATION_H */
