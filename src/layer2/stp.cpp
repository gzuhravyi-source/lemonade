#include "layer2/stp.h"
#include "core/socket.h"
#include <iostream>
#include <cstring>
namespace lemonade {
struct stp_bpdu {
    uint16_t protocol_id;
    uint8_t version;
    uint8_t type;
    uint8_t flags;
    uint64_t root_id;
    uint32_t root_path_cost;
    uint64_t bridge_id;
    uint16_t port_id;
    uint16_t message_age;
    uint16_t max_age;
    uint16_t hello_time;
    uint16_t forward_delay;
} __attribute__((packed));
void STP::map(const std::vector<std::string>& args) {
    if(args.size()<1){ std::cerr<<"Usage: -stpmap <interface>\n"; return; }
    std::string iface=args[0];
    RawSocket sock(iface);
    if(!sock.open()){ std::cerr<<"socket failed\n"; return; }
    std::cout<<"Listening for STP BPDUs on "<<iface<<"\n";
    while(true){
        auto data=sock.receive(5000);
        if(data.empty()) continue;
        if(data.size()<14+4) continue;
        if(data[12]==0x01 && data[13]==0x80) ; // STP multicast
        else continue;
        if(data.size()>=14+sizeof(stp_bpdu)){
            stp_bpdu* bpdu=(stp_bpdu*)(data.data()+14);
            std::cout<<"Root ID: "<<bpdu->root_id<<" Bridge ID: "<<bpdu->bridge_id<<"\n";
        }
    }
}
}
