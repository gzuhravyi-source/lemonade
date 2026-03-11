#include "advanced/torcheck.h"
#include <iostream>
#include <fstream>
#include <set>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
namespace lemonade {
std::set<uint32_t> load_tor_exits(){
    std::set<uint32_t> ips;
    std::ifstream f("data/tor_exit_nodes.txt");
    std::string line;
    while(std::getline(f,line)){
        ips.insert(ip_to_int(line));
    }
    return ips;
}
void TorCheck::check(const std::vector<std::string>& args) {
    if(args.size()<1){ std::cerr<<"Usage: -torcheck <ip>\n"; return; }
    std::string ip=args[0];
    static auto tor_ips=load_tor_exits();
    uint32_t ip_int=ip_to_int(ip);
    if(tor_ips.count(ip_int)) std::cout<<"IP is a known Tor exit node\n";
    else std::cout<<"IP not in Tor exit list\n";
}
}
