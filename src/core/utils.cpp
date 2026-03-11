#include "core/utils.h"
#include <arpa/inet.h>
#include <cstring>
#include <sstream>
#include <iomanip>
namespace lemonade {
uint32_t ip_to_int(const std::string& ip) { return inet_addr(ip.c_str()); }
std::string int_to_ip(uint32_t ip) { struct in_addr addr; addr.s_addr=ip; return inet_ntoa(addr); }
std::string mac_to_string(const uint8_t* mac) {
    std::ostringstream oss;
    for(int i=0;i<6;i++) oss<<std::hex<<std::setfill('0')<<std::setw(2)<<(int)mac[i]<<(i<5?":":"");
    return oss.str();
}
bool string_to_mac(const std::string& str, uint8_t* mac) {
    unsigned int a[6];
    if(sscanf(str.c_str(), "%02x:%02x:%02x:%02x:%02x:%02x", &a[0],&a[1],&a[2],&a[3],&a[4],&a[5])!=6) return false;
    for(int i=0;i<6;i++) mac[i]=a[i];
    return true;
}
uint16_t checksum(uint16_t* buf, int len) {
    uint32_t sum=0;
    for(;len>1;len-=2) sum+=*buf++;
    if(len==1) sum+=*(uint8_t*)buf;
    sum=(sum>>16)+(sum&0xffff);
    sum+=(sum>>16);
    return ~sum;
}
}
