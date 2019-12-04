#include <fstream>
#include <vector>
#include <string>
#include <iostream>


void makeTable()
{
   // Hmmmmmmmmmm  Not smart
   for(auto ene = 2.3; ene < 20.1; ene += 0.1){
      std::ifstream fin(Form("Raw/%2.1f.dat", ene));
      std::ofstream fout(Form("%2.1f.ssv", ene));
      std::string buf;
      while(std::getline(fin, buf)){
         if(buf.find("Inputs to RKIN Code") != std::string::npos){
            std::getline(fin, buf);
            std::string key = "</th></tr><tr><td>";
            auto pos = buf.find(key);
            buf.replace(0, pos + key.length(), "");
            key = " ";
            while ((pos = buf.find(key)) != std::string::npos) {
               buf.replace(pos, key.length(), "");
            }
      
            key = "</td></tr><tr><td>";
            while ((pos = buf.find(key)) != std::string::npos) {
               buf.replace(pos, key.length(), "");
            }
      
            key = "</td><td>";
            while ((pos = buf.find(key)) != std::string::npos) {
               buf.replace(pos, key.length(), " ");
            }
      
            fout << buf <<"\n";
            break;
         }
      }
      while(std::getline(fin, buf)){
         if(buf.find("</td></tr></table><p>") != std::string::npos) break;

         std::string key = " ";
         std::string::size_type pos;
         while ((pos = buf.find(key)) != std::string::npos) {
            buf.replace(pos, key.length(), "");
         }
      
         key = "</td></tr><tr><td>";
         while ((pos = buf.find(key)) != std::string::npos) {
            buf.replace(pos, key.length(), "");
         }
      
         key = "</td><td>";
         while ((pos = buf.find(key)) != std::string::npos) {
            buf.replace(pos, key.length(), " ");
         }
      
         fout << buf <<"\n";
      }
      fout.close();
      fin.close();
   }
}
