std::vector<std::string> split(std::string str, std::string delimiter)
{
  std::vector<std::string> v;
  if (str.size() == 0) return v;

  size_t start = str.find_first_not_of(delimiter);
  size_t stop;
  while ((stop = str.find_first_of(delimiter, start)) != std::string::npos) {
    v.push_back(str.substr(start, stop - start));
    start = str.find_first_not_of(delimiter, stop);
  }
  stop = str.size();
  v.push_back(str.substr(start, stop - start));

  return v;
}


std::vector<TGraph *> grXSection; // x-axis is energy
std::vector<TGraph *> grAssymmetry;
std::vector<TGraph *> grVec;

void MakeResults()
{
   std::ifstream fin("data.dat");
   std::string buf = "";

   std::vector<Double_t> angle = {0., 10., 20., 30., 40., 50., 60., 70., 80., 90.,
                                  100., 110., 120., 130., 140., 150., 160., 170., 180.};
   std::vector<Double_t> energy;

   std::vector<std::vector<Double_t>> xSection;
   std::vector<std::vector<Double_t>> assymmetry;
   std::vector<Double_t> xSecBuf;
   std::vector<Double_t> assyBuf;

   // Reading data from file
   while(std::getline(fin, buf)){
      auto vec = split(buf, " ");
      if(vec[0] == "EL="){
         energy.push_back(std::stod(vec[1]));
         xSecBuf.clear();
         assyBuf.clear();
      } else {
         xSecBuf.push_back(std::stod(vec[1]));
         assyBuf.push_back(std::stod(vec[2]));
         if(vec[0] == "180.0"){
            xSection.push_back(xSecBuf);
            assymmetry.push_back(assyBuf);
         }
      }
   }

   // Making graphs at each degrees
   for(auto iAng = 0; iAng < angle.size(); iAng++){
      std::vector<Double_t> data;
      data.clear();
      for(auto iEne = 0; iEne < energy.size(); iEne++) {
         data.push_back(xSection[iEne][iAng]);
      }

      auto gr = new TGraph(energy.size(), &energy[0], &data[0]);
      gr->SetTitle(Form("%3.0fdeg", angle[iAng]));
      grXSection.push_back(gr);
   }


   const auto nAngles = angle.size();
   Double_t val[nAngles];
   for(auto ene = 2.25; ene < 20.001; ene += 0.01){
      for(auto i = 0; i < nAngles; i++){
         // val[i] = grXSection[i]->Eval(ene, nullptr, "S");
         val[i] = grXSection[i]->Eval(ene);
      }

      auto gr = new TGraph(nAngles, &angle[0], val);
      gr->SetTitle(Form("%2.2fMeV", ene));

      auto fitFnc = new TF1("fitFnc", "pol9");
      fitFnc->SetRange(0., 180.);
      gr->Fit("fitFnc", "R");
      
      grVec.push_back(gr);
   }

   auto output = new TFile("results.root", "RECREATE");
   for(auto i = 0; i < grVec.size(); i++){
      auto gr = grVec[i];
      auto fnc = gr->GetFunction("fitFnc");
      auto chi2 = fnc->GetChisquare();
      
      if(chi2 > 1.e-10) cout << i <<"\t"<< gr->GetTitle() << endl;

      gr->Write(gr->GetTitle()); // Why does TGraph::Write() not use the title?
   }
   output->Close();
   delete output;
   
   /*
   auto canv = new TCanvas();
   canv->Print("fit.pdf[", "pdf");
   for(auto &&gr: grVec){
      gr->Draw("AL");
      canv->Print("fit.pdf", "pdf");
   }
   canv->Print("fit.pdf]", "pdf");
   */ 
}
