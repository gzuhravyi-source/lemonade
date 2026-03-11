#include "apple/darkwake.h"
#include "layer2/arp.h"
void lemonade::DarkWake::arp_nudge(const std::vector<std::string>& args){
    // reuse ARP gratuitous or poison? We'll just call ARP gratuitous
    std::vector<std::string> arp_args;
    if(args.size()>=2){
        arp_args.push_back(args[0]); // target_ip
        arp_args.push_back(args[1]); // interface
        ARP::gratuitous(arp_args);
    } else {
        std::cerr<<"Usage: -darkwake <target_ip> <interface>\n";
    }
}
