#include "perf/throughput.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
namespace lemonade {
void Throughput::saturate(const std::vector<std::string>& args) {
    if(args.size()<3){ std::cerr<<"Usage: -thruput <ip> <port> <bandwidth_kbps>\n"; return; }
    std::string ip=args[0];
    int port=std::stoi(args[1]);
    int bw=std::stoi(args[2]);
    int sock=socket(AF_INET,SOCK_DGRAM,0);
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=ip_to_int(ip);
    addr.sin_port=htons(port);
    int pkt_size=1400;
    int pkts_per_sec=(bw*1024/8)/pkt_size;
    if(pkts_per_sec<1) pkts_per_sec=1;
    auto start=std::chrono::steady_clock::now();
    long long total_bytes=0;
    char* data=new char[pkt_size];
    memset(data,'A',pkt_size);
    while(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now()-start).count()<10){
        auto cycle_start=std::chrono::steady_clock::now();
        for(int i=0;i<pkts_per_sec;i++){
            sendto(sock,data,pkt_size,0,(struct sockaddr*)&addr,sizeof(addr));
            total_bytes+=pkt_size;
        }
        auto elapsed=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-cycle_start).count();
        if(elapsed<1000000) std::this_thread::sleep_for(std::chrono::microseconds(1000000-elapsed));
    }
    delete[] data;
    close(sock);
    double throughput=(total_bytes*8.0)/(10.0*1024*1024);
    std::cout<<"Actual throughput: "<<throughput<<" Mbps\n";
}
}
