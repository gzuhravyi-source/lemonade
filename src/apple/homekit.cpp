#include "apple/homekit.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
namespace lemonade {
void HomeKit::probe(const std::vector<std::string>& args) {
    if(args.size()<1){ std::cerr<<"Usage: -homekit <ip>\n"; return; }
    std::string ip=args[0];
    int sock=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=ip_to_int(ip);
    addr.sin_port=htons(80); // typically
    if(connect(sock,(struct sockaddr*)&addr,sizeof(addr))<0){ perror("connect"); close(sock); return; }
    const char* req="GET /accessories HTTP/1.1\r\nHost: \r\n\r\n";
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
