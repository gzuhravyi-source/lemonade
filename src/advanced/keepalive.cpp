#include "advanced/keepalive.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
namespace lemonade {
void KeepAlive::test(const std::vector<std::string>& args) {
    if(args.size()<3){ std::cerr<<"Usage: -keepalive <ip> <port> <probe_interval_sec>\n"; return; }
    std::string ip=args[0];
    int port=std::stoi(args[1]);
    int interval=std::stoi(args[2]);
    int sock=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=ip_to_int(ip);
    addr.sin_port=htons(port);
    if(connect(sock,(struct sockaddr*)&addr,sizeof(addr))<0){ perror("connect"); close(sock); return; }
    int keepalive=1;
    setsockopt(sock,SOL_SOCKET,SO_KEEPALIVE,&keepalive,sizeof(keepalive));
    int idle=interval;
    setsockopt(sock,IPPROTO_TCP,TCP_KEEPIDLE,&idle,sizeof(idle));
    int count=3;
    setsockopt(sock,IPPROTO_TCP,TCP_KEEPCNT,&count,sizeof(count));
    int intvl=1;
    setsockopt(sock,IPPROTO_TCP,TCP_KEEPINTVL,&intvl,sizeof(intvl));
    std::cout<<"Keep-alive set. Monitoring for 30 seconds...\n";
    std::this_thread::sleep_for(std::chrono::seconds(30));
    close(sock);
}
}
