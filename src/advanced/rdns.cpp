#include "advanced/rdns.h"
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <arpa/inet.h>
#include <netdb.h>
namespace lemonade {
void RDNS::batch(const std::vector<std::string>& args) {
    if(args.size()<1){ std::cerr<<"Usage: -rdns <ip_range>\n"; return; }
    std::string range=args[0];
    // simple CIDR not implemented
    std::cout<<"Reverse DNS batch not implemented\n";
}
}
