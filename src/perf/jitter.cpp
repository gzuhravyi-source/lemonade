#include "perf/jitter.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
namespace lemonade {
void Jitter::calculate(const std::vector<std::string>& args) {
    if(args.size()<3){ std::cerr<<"Usage: -jitter <ip> <port> <count>\n"; return; }
    std::string ip=args[0];
    int port=std::stoi(args[1]);
    int count=std::stoi(args[2]);
    int sock=socket(AF_INET,SOCK_DGRAM,0);
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=ip_to_int(ip);
    addr.sin_port=htons(port);
    std::vector<double> rtts;
    for(int i=0;i<count;i++){
        auto start=std::chrono::high_resolution_clock::now();
        sendto(sock,"ping",4,0,(struct sockaddr*)&addr,sizeof(addr));
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(sock,&fds);
        struct timeval tv={1,0};
        if(select(sock+1,&fds,NULL,NULL,&tv)>0){
            char buf[100];
            socklen_t addrlen=sizeof(addr);
            recvfrom(sock,buf,sizeof(buf),0,(struct sockaddr*)&addr,&addrlen);
            auto end=std::chrono::high_resolution_clock::now();
            double rtt=std::chrono::duration<double,std::milli>(end-start).count();
            rtts.push_back(rtt);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    close(sock);
    if(rtts.size()>1){
        double sum=0;
        for(size_t i=1;i<rtts.size();i++) sum+=std::abs(rtts[i]-rtts[i-1]);
        double jitter=sum/(rtts.size()-1);
        std::cout<<"Average jitter: "<<jitter<<" ms\n";
    }
}
}
