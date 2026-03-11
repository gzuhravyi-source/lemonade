#include "forensic/bogon.h"
#include <iostream>
#include <arpa/inet.h>
namespace lemonade {
bool is_bogon(uint32_t ip){
    uint8_t a=(ip>>24)&0xFF;
    uint8_t b=(ip>>16)&0xFF;
    uint8_t c=(ip>>8)&0xFF;
    if(a==0) return true;
    if(a==10) return true;
    if(a==127) return true;
    if(a==169 && b==254) return true;
    if(a==172 && b>=16 && b<=31) return true;
    if(a==192 && b==168) return true;
    if(a==224) return true;
    return false;
}
void Bogon::check(const std::vector<std::string>& args) {
    if(args.size()<1){ std::cerr<<"Usage: -bogon <ip>\n"; return; }
    std::string ip=args[0];
    uint32_t ip_int=ip_to_int(ip);
    if(is_bogon(ip_int)) std::cout<<"Bogon IP\n";
    else std::cout<<"Not bogon\n";
}
}
