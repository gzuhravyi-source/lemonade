#include "layer2/mac.h"
#include "layer2/arp.h"
#include "core/socket.h"
#include "core/utils.h"
#include <iostream>
#include <fstream>
#include <map>
#include <cstring>
#include <thread>
#include <chrono>
#include <arpa/inet.h>
namespace lemonade {
static std::map<std::string,std::string> oui_db;
static std::mutex oui_mutex;
void load_oui() {
    std::lock_guard<std::mutex> lock(oui_mutex);
    if(!oui_db.empty()) return;
    std::ifstream f("data/oui.txt");
    std::string line;
    while(std::getline(f,line)){
        if(line.length()>7) oui_db[line.substr(0,8)]=line.substr(9);
    }
}
void MAC::lookup(const std::vector<std::string>& args){
    if(args.empty()){ std::cerr<<"Usage: -maclookup <MAC>\n"; return; }
    load_oui();
    std::string mac=args[0];
    if(mac.length()>=8){
        auto it=oui_db.find(mac.substr(0,8));
        if(it!=oui_db.end()) std::cout<<it->second<<"\n";
        else std::cout<<"Unknown\n";
    }
}
bool MAC::resolve(const std::string& ip, uint8_t* mac, const std::string& iface, int timeout_ms){
    RawSocket sock(iface);
    if(!sock.open()) return false;
    uint8_t broadcast[6]={0xff,0xff,0xff,0xff,0xff,0xff};
    arp_packet pkt;
    pkt.htype=htons(1); pkt.ptype=htons(0x0800); pkt.hlen=6; pkt.plen=4; pkt.op=htons(1);
    memcpy(pkt.sender_mac, sock.get_mac(), 6);
    pkt.sender_ip=ip_to_int("0.0.0.0");
    memcpy(pkt.target_mac, broadcast, 6);
    pkt.target_ip=ip_to_int(ip);
    std::vector<uint8_t> frame(14+sizeof(pkt));
    memcpy(frame.data(), broadcast,6);
    memcpy(frame.data()+6, sock.get_mac(),6);
    frame[12]=0x08; frame[13]=0x06;
    memcpy(frame.data()+14, &pkt, sizeof(pkt));
    sock.send(frame);
    auto start=std::chrono::steady_clock::now();
    while(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()-start).count()<timeout_ms){
        auto data=sock.receive(100);
        if(data.empty()) continue;
        if(data.size()<14+sizeof(arp_packet)) continue;
        if(data[12]!=0x08 || data[13]!=0x06) continue;
        arp_packet* resp=(arp_packet*)(data.data()+14);
        if(resp->op==htons(2) && resp->sender_ip==ip_to_int(ip)){
            memcpy(mac, resp->sender_mac, 6);
            return true;
        }
    }
    return false;
}
}
