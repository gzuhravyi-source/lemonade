#include "layer2/vlan.h"
#include "core/socket.h"
#include "core/utils.h"
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
namespace lemonade {
struct vlan_header {
    uint16_t tpid;
    uint16_t tci;
} __attribute__((packed));
void VLAN::inject(const std::vector<std::string>& args) {
    if(args.size()<3){ std::cerr<<"Usage: -vlaninject <interface> <vlan_id> <target_mac>\n"; return; }
    std::string iface=args[0];
    uint16_t vlan_id=std::stoi(args[1]) & 0xFFF;
    std::string target_mac_str=args[2];
    uint8_t target_mac[6];
    if(!string_to_mac(target_mac_str,target_mac)){ std::cerr<<"Invalid MAC\n"; return; }
    RawSocket sock(iface);
    if(!sock.open()){ std::cerr<<"socket failed\n"; return; }
    std::vector<uint8_t> frame(14+4+20); // ethernet + vlan + ip header placeholder
    memcpy(frame.data(), target_mac,6);
    memcpy(frame.data()+6, sock.get_mac(),6);
    frame[12]=0x81; frame[13]=0x00;
    vlan_header vh;
    vh.tpid=htons(0x8100);
    vh.tci=htons(vlan_id);
    memcpy(frame.data()+14, &vh, 4);
    // fake IP payload
    sock.send(frame);
    std::cout<<"VLAN tagged frame sent\n";
}
}
