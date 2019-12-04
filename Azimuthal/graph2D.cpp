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


TGraph2D *grXSection;
TGraph2D *grAssymmetry;

std::vector<TGraph *> grVec;

void graph2D()
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

   grXSection = new TGraph2D();
   grAssymmetry = new TGraph2D();
   const auto nData = angle.size() * energy.size();
   for(auto i = 0; i < nData; i++){
      auto x = energy[i % energy.size()];
      auto y = angle[i % angle.size()];

      grXSection->SetPoint(i, x, y, xSection[i % energy.size()][i % angle.size()]);
      grAssymmetry->SetPoint(i, x, y, assymmetry[i % energy.size()][i % angle.size()]);
   }

   grXSection->Draw("SURF1");

   // making histogram is better?
   // To generate the random number, TF or TH are needed
   auto output = new TFile("azimuthal.root", "RECREATE");
   grXSection->Write("crossSection");
   grAssymmetry->Write("assymmetry");
   output->Close();

   const auto nAngles = angle.size();
   Double_t val[nAngles];
   for(auto ene = 2.25; ene <= 20.; ene += 0.25){
      for(auto i = 0; i < nAngles; i++){
         val[i] = grXSection->Interpolate(ene, angle[i]);
      }

      auto gr = new TGraph(nAngles, &angle[0], val);
      gr->SetTitle(Form("%2.2fMeV", ene));

      auto fitFnc = new TF1("fitFnc", "pol9");
      fitFnc->SetRange(0., 180.);
      gr->Fit("fitFnc", "R");
      
      grVec.push_back(gr);
   }


   for(auto i = 0; i < grVec.size(); i++){
      auto gr = grVec[i];
      auto fnc = gr->GetFunction("fitFnc");
      auto chi2 = fnc->GetChisquare();
      
      if(chi2 > 1.e-6) cout << i <<"\t"<< gr->GetTitle() << endl;
   }
   
}
