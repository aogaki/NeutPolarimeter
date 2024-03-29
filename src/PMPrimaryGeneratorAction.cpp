#include <TFile.h>
#include <TGraph.h>
#include <TRandom3.h>

#include <G4AutoLock.hh>
#include <G4ChargedGeantino.hh>
#include <G4IonTable.hh>
#include <G4ParticleTable.hh>
#include <G4SystemOfUnits.hh>
#include <Randomize.hh>
#include <g4root.hh>
#include <random>

#include "PMPrimaryGeneratorAction.hpp"

static G4int nEveInPGA = 0;
G4Mutex mutexConst = G4MUTEX_INITIALIZER;
G4Mutex mutexGen = G4MUTEX_INITIALIZER;
G4Mutex mutexCounter = G4MUTEX_INITIALIZER;

PMPrimaryGeneratorAction::PMPrimaryGeneratorAction(G4double beamEne,
                                                   G4bool unpolarizedFlag)
    : G4VUserPrimaryGeneratorAction()
{
  fUnpolarizedFlag = unpolarizedFlag;
  fGammaEne = beamEne * MeV;
  fGammaSigma = fGammaEne * 0.005 / (2 * sqrt(2 * log(2)));

  fParticleGun.reset(new G4ParticleGun(1));
  auto particleTable = G4ParticleTable::GetParticleTable();
  auto particle = particleTable->FindParticle("neutron");
  fParticleGun->SetParticleDefinition(particle);

  G4AutoLock lock(&mutexConst);
  fAngDist.reset(new TF2("angdist",
                         "sin(x)*([0]+[1]*pow(sin(x),2)+[2]*pow(sin(2*x),2)-"
                         "cos(2*y)*([1]*pow(sin(x),2)+[2]*pow(sin(2*x),2)))",
                         0, CLHEP::pi, 0, CLHEP::pi2));
  fAngDist->SetParameter(0, 0.01);
  fAngDist->SetParameter(1, 0.99);
  fAngDist->SetParameter(2, 0.);
  fAngDist->SetNpx(1000);
  fAngDist->SetNpy(1000);
  // fAngDist->SetRange(0., 0., CLHEP::pi, CLHEP::pi2);
  fAngDist->SetRange(80. * deg, 0., 100. * deg, CLHEP::pi2);

  std::random_device rndSeed;
  gRandom->SetSeed(rndSeed());

  auto file = new TFile("KEneAng.root", "READ");
  fKEneAng.reset((TGraph2D *)file->Get("KEneAng"));
  fKEneAng->SetDirectory(0);
  file->Close();
  delete file;

  file = new TFile("AzimResults.root", "READ");
  auto graph = (TGraph *)file->Get(Form("%2.2fMeV", beamEne));
  fAzimuthal.reset(graph->GetFunction("fitFnc"));
  file->Close();
  delete file;
}

PMPrimaryGeneratorAction::~PMPrimaryGeneratorAction() {}

void PMPrimaryGeneratorAction::GenBeam()
{
  G4AutoLock lock(&mutexGen);
  fAngDist->GetRandom2(fBeamTheta, fBeamPhi);
  fBeamEne = fKEneAng->Interpolate(fBeamTheta * 180 / CLHEP::pi,
                                   gRandom->Gaus(fGammaEne, fGammaSigma));
}

void PMPrimaryGeneratorAction::GeneratePrimaries(G4Event *event)
{
  auto beamR = 1. * sqrt(G4UniformRand()) * cm;  // Uniform in 2cm dia
  auto posTheta = G4UniformRand() * CLHEP::pi2;
  auto beamPos = G4ThreeVector(beamR * cos(posTheta), beamR * sin(posTheta),
                               (G4UniformRand() * 3.7 - 3.7 / 2.0) * cm);
  // GenBeam();

  fAngDist->GetRandom2(fBeamTheta, fBeamPhi);
  // auto angLimit = 20. * deg;
  // fBeamTheta = acos(1. - G4UniformRand() * (1. - cos(angLimit))) + 80. * deg;
  //
  // if (fUnpolarizedFlag) {
  //   fBeamPhi = G4UniformRand() * CLHEP::pi2;
  // } else {
  //   G4AutoLock lock(&mutexGen);
  //   fBeamPhi = fAzimuthal->GetRandom() * deg;
  //   lock.unlock();
  // }
  // if (gRandom->Integer(2) == 0) fBeamPhi = -fBeamPhi;

  auto beamP = G4ThreeVector(sin(fBeamTheta) * cos(fBeamPhi),
                             sin(fBeamTheta) * sin(fBeamPhi), cos(fBeamTheta));

  fBeamEne = fKEneAng->Interpolate(fBeamTheta * 180 / CLHEP::pi,
                                   gRandom->Gaus(fGammaEne, fGammaSigma));

  fParticleGun->SetParticleEnergy(fBeamEne);
  fParticleGun->SetParticlePosition(beamPos);
  fParticleGun->SetParticleMomentumDirection(beamP);
  fParticleGun->GeneratePrimaryVertex(event);

  G4AutoLock lock(&mutexCounter);
  if (nEveInPGA++ % 10000 == 0)
    G4cout << nEveInPGA - 1 << " events done" << G4endl;
}
