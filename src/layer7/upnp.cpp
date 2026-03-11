#include "layer7/upnp.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
namespace lemonade {
void UPNP::discover(const std::vector<std::string>& args) {
    if(args.size()<1){ std::cerr<<"Usage: -upnpdisc <interface>\n"; return; }
    std::string iface=args[0];
    int sock=socket(AF_INET,SOCK_DGRAM,0);
    int broadcast=1;
    setsockopt(sock,SOL_SOCKET,SO_BROADCAST,&broadcast,sizeof(broadcast));
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=inet_addr("239.255.255.250");
    addr.sin_port=htons(1900);
    const char* msearch="M-SEARCH * HTTP/1.1\r\nHOST:239.255.255.250:1900\r\nMAN:\"ssdp:discover\"\r\nMX:3\r\nST:ssdp:all\r\n\r\n";
    sendto(sock,msearch,strlen(msearch),0,(struct sockaddr*)&addr,sizeof(addr));
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(sock,&fds);
    struct timeval tv={5,0};
    while(select(sock+1,&fds,NULL,NULL,&tv)>0){
        char buf[2048];
        socklen_t addrlen=sizeof(addr);
        int n=recvfrom(sock,buf,sizeof(buf)-1,0,(struct sockaddr*)&addr,&addrlen);
        if(n>0){
            buf[n]=0;
            std::cout<<buf<<"\n";
        }
    }
    close(sock);
}
}
