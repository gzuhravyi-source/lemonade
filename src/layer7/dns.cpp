#include "layer7/dns.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
namespace lemonade {
void DNS::recursive_check(const std::vector<std::string>& args) {
    if(args.size()<2){ std::cerr<<"Usage: -dnsrec <dns_server> <domain>\n"; return; }
    std::string server=args[0];
    std::string domain=args[1];
    int sock=socket(AF_INET,SOCK_DGRAM,0);
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=ip_to_int(server);
    addr.sin_port=htons(53);
    unsigned char packet[512];
    memset(packet,0,sizeof(packet));
    packet[0]=rand()%256; packet[1]=rand()%256; // id
    packet[2]=0x01; packet[3]=0x00; // recursion desired
    packet[4]=0x00; packet[5]=0x01; // questions: 1
    // encode domain
    int pos=12;
    const char* dom=domain.c_str();
    const char* lab=dom;
    while(*dom){
        if(*dom=='.'){
            int len=dom-lab;
            packet[pos++]=len;
            memcpy(packet+pos,lab,len);
            pos+=len;
            lab=dom+1;
        }
        dom++;
    }
    int len=dom-lab;
    packet[pos++]=len;
    memcpy(packet+pos,lab,len);
    pos+=len;
    packet[pos++]=0; // end
    packet[pos++]=0x00; packet[pos++]=0x01; // type A
    packet[pos++]=0x00; packet[pos++]=0x01; // class IN
    sendto(sock,packet,pos,0,(struct sockaddr*)&addr,sizeof(addr));
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(sock,&fds);
    struct timeval tv={3,0};
    if(select(sock+1,&fds,NULL,NULL,&tv)>0){
        unsigned char recvbuf[512];
        socklen_t addrlen=sizeof(addr);
        int n=recvfrom(sock,recvbuf,sizeof(recvbuf),0,(struct sockaddr*)&addr,&addrlen);
        if(n>0 && (recvbuf[3] & 0x0F)!=0){
            std::cout<<"Server is recursive? RCODE="<<(recvbuf[3] & 0x0F)<<"\n";
        } else {
            std::cout<<"Server may be open\n";
        }
    }
    close(sock);
}
}
