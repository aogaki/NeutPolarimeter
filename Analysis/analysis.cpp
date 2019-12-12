#include <TCanvas.h>
#include <TFile.h>
#include <TGraph.h>
#include <TH1.h>
#include <TH2.h>
#include <TMultiGraph.h>
#include <TString.h>
#include <TTree.h>

#include <iostream>
#include <memory>
#include <vector>

class THit
{
 public:
  THit() { fTime = fDepEne = 0.; };
  THit(Double_t ene, Double_t time)
  {
    fDepEne = ene;
    fTime = time;
  };

  Double_t fTime;
  Double_t fDepEne;
};

// cfdVal [0, 1].  0 is first hit.
Double_t GetTrgTime(std::vector<THit> v, Double_t cfdVal)
{
  Double_t sumEne = 0.;

  for (auto &&hit : v) sumEne += hit.fDepEne;

  auto th = sumEne * cfdVal;

  const auto kHit = v.size();
  auto sum = 0.;  // hmm stupid name?
  auto trgTime = 0.;
  for (auto iHit = 0; iHit < kHit; iHit++) {
    sum += v[iHit].fDepEne;
    if (sum > th) {
      // No interpolation or something to calculate
      trgTime = v[iHit].fTime;
      break;
    }
  }

  return trgTime;
}

void InitHists(TH1D *&histIn, TH1D *&histOut, TH1D *&histMiddle, Double_t ene)
{
  auto index = Int_t(ene);
  histIn = new TH1D(Form("histIn%d", index), Form("In-plane: %2.2f MeV", ene),
                    2000, 0., 20.);
  histIn->SetDirectory(nullptr);
  histIn->SetXTitle("[MeV]");

  histOut = new TH1D(Form("histOut%d", index),
                     Form("Out-plane: %2.2f MeV", ene), 2000, 0., 20.);
  histOut->SetDirectory(nullptr);
  histOut->SetXTitle("[MeV]");

  histMiddle = new TH1D(Form("histMiddle%d", index),
                        Form("Middle-plane: %2.2f MeV", ene), 2000, 0., 20.);
  histMiddle->SetDirectory(nullptr);
  histMiddle->SetXTitle("[MeV]");
}

std::vector<TH1D *> In;
std::vector<TH1D *> Out;
std::vector<TH1D *> Middle;

TH2D *histTest;

TMultiGraph *mg;

void analysis()
{
  histTest =
      new TH2D("histTest", "time VS energy", 100, 0., 10., 100, 0., 100.);

  mg = new TMultiGraph();
  auto grIn = new TGraph();
  auto grOut = new TGraph();
  auto grMiddle = new TGraph();
  auto grDataIndex = 0;

  // for (auto ene = 3; ene <= 20; ene++) {
  for (auto ene = 4; ene <= 20; ene++) {
    // auto ene = 10;

    TH1D *histIn = nullptr;
    TH1D *histOut = nullptr;
    TH1D *histMiddle = nullptr;
    InitHists(histIn, histOut, histMiddle, ene);

    auto input = new TFile(Form("pol%d.root", ene), "READ");
    auto tree = (TTree *)input->Get("PolMeter");

    tree->SetBranchStatus("*", kFALSE);

    Char_t volName[256];
    tree->SetBranchStatus("VolName", kTRUE);
    tree->SetBranchAddress("VolName", volName);

    Int_t eveID;
    tree->SetBranchStatus("EventID", kTRUE);
    tree->SetBranchAddress("EventID", &eveID);

    Double_t depEne;
    tree->SetBranchStatus("DepositEnergy", kTRUE);
    tree->SetBranchAddress("DepositEnergy", &depEne);

    Double_t time;
    tree->SetBranchStatus("Time", kTRUE);
    tree->SetBranchAddress("Time", &time);

    Int_t pdg;
    tree->SetBranchStatus("PDGCode", kTRUE);
    tree->SetBranchAddress("PDGCode", &pdg);

    Double_t sumEneIn = 0.;
    Double_t sumEneOut = 0.;
    Double_t sumEneMiddle = 0.;
    Int_t currentID;

    tree->GetEntry(0);
    currentID = eveID;

    const auto kHit = tree->GetEntries();
    std::vector<THit> hitInfo;
    for (auto iHit = 0; iHit < kHit; iHit++) {
      tree->GetEntry(iHit);

      if (currentID != eveID) {
        if (sumEneIn > 0.) {
          auto trgTime = GetTrgTime(hitInfo, 0.25);
          if (trgTime > 0.) histIn->Fill(sumEneIn);
          if (ene == 20) histTest->Fill(sumEneIn, trgTime);
        }
        if (sumEneOut > 0.) histOut->Fill(sumEneOut);
        if (sumEneMiddle > 0.) histMiddle->Fill(sumEneMiddle);

        sumEneIn = sumEneOut = sumEneMiddle = 0.;
        currentID = eveID;
        hitInfo.clear();
      }

      // if (pdg == 2212) {
      if (kTRUE) {
        auto detName = TString(volName);
        if (detName == "Detector1") {
          sumEneIn += depEne;
          hitInfo.push_back(THit(depEne, time));
        } else if (detName == "Detector0") {
          sumEneOut += depEne;
        } else if (detName == "Detector3") {
          sumEneMiddle += depEne;
        }
      }
      // std::cout << eveID << "\t" << volName << "\t" << depEne << std::endl;
    }

    In.push_back(histIn);
    Out.push_back(histOut);
    Middle.push_back(histMiddle);

    auto nEveIn = histIn->GetEntries();
    auto nEveOut = histOut->GetEntries();
    auto nEveMiddle = histMiddle->GetEntries();

    std::cout << (nEveIn - nEveOut) / (nEveIn + nEveOut) << std::endl;

    grIn->SetPoint(grDataIndex, ene, nEveIn);
    grOut->SetPoint(grDataIndex, ene, nEveOut);
    grMiddle->SetPoint(grDataIndex, ene, nEveMiddle);

    // grIn->SetPoint(grDataIndex, ene, (nEveIn - nEveOut) / (nEveIn + nEveOut));
    // grOut->SetPoint(grDataIndex, ene, histOut->GetEntries());
    // grMiddle->SetPoint(grDataIndex, ene,
    //                    (nEveMiddle - nEveOut) / (nEveMiddle + nEveOut));
    grDataIndex++;

    input->Close();
    delete input;
  }

  mg->Add(grIn);
  mg->Add(grOut);
  mg->Add(grMiddle);

  auto canv = new TCanvas();
  canv->Divide(2, 2);

  canv->cd(1);
  In[In.size() - 1]->Draw();
  canv->cd(2);
  Out[Out.size() - 1]->Draw();
  canv->cd(3);
  Middle[Middle.size() - 1]->Draw();
  canv->cd(4);
  // histTest->Draw("COL");
  mg->Draw("AL");

  auto output = new TFile("hists.root", "RECREATE");
  for (auto i = 0; i < In.size(); i++) {
    In[i]->Write();
    Out[i]->Write();
    Middle[i]->Write();
  }
  output->Close();
  delete output;
}
