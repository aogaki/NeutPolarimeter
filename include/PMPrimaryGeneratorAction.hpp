#ifndef PMPrimaryGeneratorAction_h
#define PMPrimaryGeneratorAction_h 1

#include <TF2.h>
#include <TGraph2D.h>

#include <G4Event.hh>
#include <G4GenericMessenger.hh>
#include <G4ParticleGun.hh>
#include <G4Threading.hh>
#include <G4ThreeVector.hh>
#include <G4VUserPrimaryGeneratorAction.hh>
#include <memory>

class PMPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
 public:
  PMPrimaryGeneratorAction(G4double beamEne, G4bool unpolarizedFlag);
  virtual ~PMPrimaryGeneratorAction();

  virtual void GeneratePrimaries(G4Event *);

 private:
  // G4ParticleGun *fParticleGun;
  std::unique_ptr<G4ParticleGun> fParticleGun;
  G4double fGammaEne;
  G4double fGammaSigma;
  G4bool fUnpolarizedFlag;

  void GenBeam();
  G4double fBeamEne;
  G4double fBeamTheta;
  G4double fBeamPhi;

  std::unique_ptr<TGraph2D> fKEneAng;
  std::unique_ptr<TF2> fAngDist;
  std::unique_ptr<TF1> fAzimuthal;
};

#endif
