void test()
{
   auto input = new TFile("results.root", "READ");

   auto gr = (TGraph*)input->Get("10.00MeV");
   auto f = gr->GetFunction("fitFnc");

   for(auto i = 0; i < 1000000; i++){
      auto val = f->GetRandom();
      if(val > 179.) cout << val << endl;
   }

   Double_t xmin, xmax;
   f->GetRange(xmin, xmax);
   cout << xmin <<"\t"<< xmax << endl;
   
}
