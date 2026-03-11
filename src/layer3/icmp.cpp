#include "layer3/icmp.h"
#include "core/utils.h"
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/ip_icmp.h>
namespace lemonade {
void ICMP::timestamp(const std::vector<std::string>& args) {
    if(args.size()<1){ std::cerr<<"Usage: -icmptimestamp <ip>\n"; return; }
    std::string target=args[0];
    int sockfd=socket(AF_INET,SOCK_RAW,IPPROTO_ICMP);
    if(sockfd<0){ perror("socket"); return; }
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=ip_to_int(target);
    char packet[64];
    struct icmp* icmp=(struct icmp*)packet;
    icmp->icmp_type=13; // timestamp request
    icmp->icmp_code=0;
    icmp->icmp_id=getpid();
    icmp->icmp_seq=1;
    icmp->icmp_cksum=0;
    icmp->icmp_timestamp=htonl(time(NULL)*1000);
    icmp->icmp_cksum=checksum((uint16_t*)icmp,64);
    sendto(sockfd,packet,64,0,(struct sockaddr*)&addr,sizeof(addr));
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(sockfd,&fds);
    struct timeval tv={3,0};
    if(select(sockfd+1,&fds,NULL,NULL,&tv)>0){
        char recvbuf[1500];
        socklen_t addrlen=sizeof(addr);
        int n=recvfrom(sockfd,recvbuf,sizeof(recvbuf),0,(struct sockaddr*)&addr,&addrlen);
        if(n>0){
            struct ip* ip=(struct ip*)recvbuf;
            struct icmp* icmp_resp=(struct icmp*)(recvbuf+ip->ip_hl*4);
            if(icmp_resp->icmp_type==14){ // timestamp reply
                uint32_t originate=ntohl(icmp_resp->icmp_otime);
                uint32_t receive=ntohl(icmp_resp->icmp_rtime);
                uint32_t transmit=ntohl(icmp_resp->icmp_ttime);
                std::cout<<"Originate: "<<originate<<" Receive: "<<receive<<" Transmit: "<<transmit<<"\n";
            }
        }
    } else {
        std::cout<<"No reply\n";
    }
    close(sockfd);
}
}
