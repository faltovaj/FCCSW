#ifndef SIMG4COMPONENTS_G4SAVEMCTRUTHPARTICLES_H
#define SIMG4COMPONENTS_G4SAVEMCTRUTHPARTICLES_H

// Gaudi
#include "GaudiKernel/AlgTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "SimG4Interface/IG4SaveOutputTool.h"

// datamodel
namespace fcc {
class MCParticleCollection;
class GenVertexCollection;
}

/** @class G4SaveMCTruthParticles 
 *  Save MC truth particles 
 *    -- generated primary particles
 *    -- secondary particles produced in Geant4
 *
 *  @author Jana Faltova
 */

class G4SaveMCTruthParticles: public AlgTool, virtual public IG4SaveOutputTool {
public:
  explicit G4SaveMCTruthParticles(const std::string& aType , const std::string& aName,
                  const IInterface* aParent);
  virtual ~G4SaveMCTruthParticles();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize();
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize();
  /**  Save the data output.
   *   Saves the 'reconstructed' (smeared) particles and associates them with MC particles.
   *   @param[in] aEvent Event with data to save.
   *   @return status code
   */
  virtual StatusCode saveOutput(const G4Event& aEvent) final;
private:
  /// Handle for the particles to be written
  DataHandle<fcc::MCParticleCollection> m_particles;
  //Handle for the vertices to be written
  DataHandle<fcc::GenVertexCollection> m_vertices;
  /// Handle for the associations between particles and MC particles to be written

};

#endif /* SIMG4COMPONENTS_G4SAVEMCTRUTHPARTICLES_H */
