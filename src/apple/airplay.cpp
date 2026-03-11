#include "apple/airplay.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
namespace lemonade {
void AirPlay::probe(const std::vector<std::string>& args) {
    if(args.size()<1){ std::cerr<<"Usage: -airplay <ip>\n"; return; }
    std::string ip=args[0];
    int sock=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=ip_to_int(ip);
    addr.sin_port=htons(5000);
    if(connect(sock,(struct sockaddr*)&addr,sizeof(addr))<0){ perror("connect"); close(sock); return; }
    const char* req="OPTIONS * RTSP/1.0\r\nCSeq: 1\r\n\r\n";
    send(sock,req,strlen(req),0);
    char buf[1024];
    int n=recv(sock,buf,sizeof(buf)-1,0);
    if(n>0){
        buf[n]=0;
        std::cout<<buf;
    }
    close(sock);
}
}
