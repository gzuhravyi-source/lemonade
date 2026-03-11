#include "perf/entropy.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <map>
namespace lemonade {
void Entropy::calculate(const std::vector<std::string>& args) {
    if(args.size()<1){ std::cerr<<"Usage: -entropy <file>\n"; return; }
    std::string filename=args[0];
    std::ifstream f(filename,std::ios::binary);
    std::vector<uint8_t> data((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    if(data.empty()) return;
    std::map<uint8_t,int> freq;
    for(uint8_t b:data) freq[b]++;
    double entropy=0.0;
    double len=data.size();
    for(auto& p:freq){
        double prob=p.second/len;
        entropy-=prob*log2(prob);
    }
    std::cout<<"Entropy: "<<entropy<<"\n";
}
}
