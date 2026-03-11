#include "advanced/whois.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
namespace lemonade {
void Whois::lookup(const std::vector<std::string>& args) {
    if(args.size()<1){ std::cerr<<"Usage: -whois <ip>\n"; return; }
    std::string ip=args[0];
    int sock=socket(AF_INET,SOCK_STREAM,0);
    struct hostent* server=gethostbyname("whois.arin.net");
    if(!server){ std::cerr<<"Cannot resolve whois server\n"; return; }
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    memcpy(&addr.sin_addr.s_addr,server->h_addr,server->h_length);
    addr.sin_port=htons(43);
    if(connect(sock,(struct sockaddr*)&addr,sizeof(addr))<0){ perror("connect"); close(sock); return; }
    std::string query=ip+"\r\n";
    send(sock,query.c_str(),query.size(),0);
    char buf[4096];
    int n;
    while((n=recv(sock,buf,sizeof(buf)-1,0))>0){
        buf[n]=0;
        std::cout<<buf;
    }
    close(sock);
}
}
