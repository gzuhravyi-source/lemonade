#include "advanced/socket_exhaust.h"
#include <iostream>
#include <thread>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <chrono>
namespace lemonade {
void SocketExhaust::test(const std::vector<std::string>& args) {
    if(args.size()<3){ std::cerr<<"Usage: -sockexhaust <ip> <port> <num_conn>\n"; return; }
    std::string ip=args[0];
    int port=std::stoi(args[1]);
    int num=std::stoi(args[2]);
    std::vector<int> socks;
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=ip_to_int(ip);
    addr.sin_port=htons(port);
    for(int i=0;i<num;i++){
        int sock=socket(AF_INET,SOCK_STREAM,0);
        if(sock<0) break;
        fcntl(sock,F_SETFL,fcntl(sock,F_GETFL,0)|O_NONBLOCK);
        connect(sock,(struct sockaddr*)&addr,sizeof(addr));
        socks.push_back(sock);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    std::cout<<"Opened "<<socks.size()<<" connections\n";
    std::this_thread::sleep_for(std::chrono::seconds(5));
    for(int s:socks) close(s);
}
}
