#include "forensic/flow.h"
#include <iostream>
#include <fstream>
#include <map>
#include <cstring>
#include <arpa/inet.h>
namespace lemonade {
struct flow_key {
    uint32_t src_ip, dst_ip;
    uint16_t src_port, dst_port;
    uint8_t proto;
    bool operator<(const flow_key& o) const {
        if(src_ip!=o.src_ip) return src_ip<o.src_ip;
        if(dst_ip!=o.dst_ip) return dst_ip<o.dst_ip;
        if(src_port!=o.src_port) return src_port<o.src_port;
        if(dst_port!=o.dst_port) return dst_port<o.dst_port;
        return proto<o.proto;
    }
};
void Flow::analyze(const std::vector<std::string>& args) {
    std::cout<<"Flow analysis not fully implemented\n";
}
}
