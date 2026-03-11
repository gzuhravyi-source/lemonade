#include "perf/latency.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cmath>
namespace lemonade {
void Latency::profile(const std::vector<std::string>& args) {
    if(args.size()<3){ std::cerr<<"Usage: -latency <ip> <port> <samples>\n"; return; }
    std::string ip=args[0];
    int port=std::stoi(args[1]);
    int samples=std::stoi(args[2]);
    int sock=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=ip_to_int(ip);
    addr.sin_port=htons(port);
    std::vector<double> rtts;
    for(int i=0;i<samples;i++){
        auto start=std::chrono::high_resolution_clock::now();
        if(connect(sock,(struct sockaddr*)&addr,sizeof(addr))==0){
            auto end=std::chrono::high_resolution_clock::now();
            double rtt=std::chrono::duration<double,std::milli>(end-start).count();
            rtts.push_back(rtt);
            close(sock);
            sock=socket(AF_INET,SOCK_STREAM,0);
        } else {
            close(sock);
            sock=socket(AF_INET,SOCK_STREAM,0);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    close(sock);
    if(!rtts.empty()){
        double sum=0;
        for(double r:rtts) sum+=r;
        double avg=sum/rtts.size();
        double variance=0;
        for(double r:rtts) variance+=(r-avg)*(r-avg);
        variance/=rtts.size();
        std::cout<<"Average connection latency: "<<avg<<" ms, stddev: "<<sqrt(variance)<<" ms\n";
    }
}
}
