#include "forensic/pcap.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <arpa/inet.h>
namespace lemonade {
struct pcap_global_header {
    uint32_t magic_number;
    uint16_t version_major;
    uint16_t version_minor;
    int32_t thiszone;
    uint32_t sigfigs;
    uint32_t snaplen;
    uint32_t network;
};
struct pcap_packet_header {
    uint32_t ts_sec;
    uint32_t ts_usec;
    uint32_t incl_len;
    uint32_t orig_len;
};
void PCAP::read(const std::vector<std::string>& args) {
    if(args.size()<1){ std::cerr<<"Usage: -pcapread <file>\n"; return; }
    std::string file=args[0];
    std::ifstream f(file,std::ios::binary);
    pcap_global_header gh;
    f.read((char*)&gh,sizeof(gh));
    if(gh.magic_number!=0xa1b2c3d4 && gh.magic_number!=0xd4c3b2a1){
        std::cerr<<"Not a pcap file\n"; return;
    }
    int packet_count=0;
    while(!f.eof()){
        pcap_packet_header ph;
        f.read((char*)&ph,sizeof(ph));
        if(f.gcount()!=sizeof(ph)) break;
        std::vector<char> data(ph.incl_len);
        f.read(data.data(),ph.incl_len);
        std::cout<<"Packet "<<++packet_count<<": length "<<ph.orig_len<<" captured "<<ph.incl_len<<"\n";
    }
}
}
