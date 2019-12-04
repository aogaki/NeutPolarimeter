// Rough cut the original data file
// Too much if indent...

void cut()
{
   std::ifstream fin("original.dat");
   std::string buf = "";
   std::vector<std::string> sumBuf;

   std::ofstream fout("data.dat");
   
   while(std::getline(fin, buf)){
      if(buf.find("DEUTERONPHOTODISINTEGRATION") != std::string::npos) {
         // sumBuf.push_back(buf);
         while(std::getline(fin, buf)){
            // sumBuf.push_back(buf);
            if(buf.find("KNP=") != std::string::npos) {
               if(buf.find("KNP= 6") != std::string::npos) {
                  // sumBuf.push_back(buf);                  
                  while(std::getline(fin, buf)){
                     if(buf.find("CIRCULAR-PHOTON ") != std::string::npos){
                        break;
                     }
                     if(buf.find("PHOTON LAB ENERGY") == 0){
                        buf.replace(0, 18, "");
                        buf.replace(buf.find(" ["), 100, "");
                        sumBuf.push_back(buf);
                     } else if(buf.find("0.0") == 0 ||
                             buf.find("10.0") == 0 ||
                             buf.find("20.0") == 0 ||
                             buf.find("30.0") == 0 ||
                             buf.find("40.0") == 0 ||
                             buf.find("50.0") == 0 ||
                             buf.find("60.0") == 0 ||
                             buf.find("70.0") == 0 ||
                             buf.find("80.0") == 0 ||
                             buf.find("90.0") == 0 ||
                             buf.find("100.0") == 0 ||
                             buf.find("110.0") == 0 ||
                             buf.find("120.0") == 0 ||
                             buf.find("130.0") == 0 ||
                             buf.find("140.0") == 0 ||
                             buf.find("150.0") == 0 ||
                             buf.find("160.0") == 0 ||
                             buf.find("170.0") == 0 ||
                             buf.find("180.0") == 0 )
                        sumBuf.push_back(buf);
                  }
                  for(auto line: sumBuf)
                     fout << line << endl;

                  sumBuf.clear();

               } else {
                  sumBuf.clear();
                  break;
               }
            }
         }
      }
   }
         
   fout.close();
}
