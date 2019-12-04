TGraph2D *gr;

void makeGraph()
{
   std::vector<Double_t> x;
   std::vector<Double_t> y;
   std::vector<Double_t> z;
   
   for(auto ene = 2.3; ene < 20.1; ene += 0.1){
      std::ifstream fin(Form("%2.1f.ssv", ene));
      Double_t ang, KE, buf;

      while(fin >> ang >> buf >> KE >> buf >> buf >> buf){
         cout << ang <<"\t"<< KE << endl;
         x.push_back(ang);
         y.push_back(ene);
         z.push_back(KE);
      }
   }

   gr = new TGraph2D(x.size(), x.data(), y.data(), z.data());
   gr->Draw();

   auto file = new TFile("KEneAng.root", "RECREATE");
   gr->Write("KEneAng");
   file->Close();
}
