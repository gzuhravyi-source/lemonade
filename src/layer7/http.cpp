#include "layer7/http.h"
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <netdb.h>
namespace lemonade {
void HTTP::fuzz(const std::vector<std::string>& args) {
    if(args.size()<2){ std::cerr<<"Usage: -httpfuzz <url> <wordlist>\n"; return; }
    std::string url=args[0];
    std::string wordlist=args[1];
    std::ifstream wl(wordlist);
    std::string line;
    std::string host;
    std::string path="/";
    size_t pos=url.find("://");
    if(pos!=std::string::npos) url=url.substr(pos+3);
    pos=url.find('/');
    if(pos!=std::string::npos){
        host=url.substr(0,pos);
        path=url.substr(pos);
    } else {
        host=url;
    }
    while(std::getline(wl,line)){
        int sock=socket(AF_INET,SOCK_STREAM,0);
        struct hostent* server=gethostbyname(host.c_str());
        if(!server){ std::cerr<<"Host not found\n"; return; }
        struct sockaddr_in addr;
        addr.sin_family=AF_INET;
        memcpy(&addr.sin_addr.s_addr,server->h_addr,server->h_length);
        addr.sin_port=htons(80);
        if(connect(sock,(struct sockaddr*)&addr,sizeof(addr))<0){ close(sock); continue; }
        std::string request="GET "+path+" HTTP/1.1\r\nHost: "+host+"\r\nUser-Agent: Mozilla/5.0\r\nX-Fuzz: "+line+"\r\n\r\n";
        send(sock,request.c_str(),request.size(),0);
        char buf[1024];
        int n=recv(sock,buf,sizeof(buf)-1,0);
        if(n>0){
            buf[n]=0;
            std::cout<<"Response for fuzz '"<<line<<"':\n"<<buf<<"\n";
        }
        close(sock);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
void HTTP::banner_grab(const std::vector<std::string>& args) {
    if(args.size()<2){ std::cerr<<"Usage: -banner <ip> <port>\n"; return; }
    std::string ip=args[0];
    int port=std::stoi(args[1]);
    int sock=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=ip_to_int(ip);
    addr.sin_port=htons(port);
    if(connect(sock,(struct sockaddr*)&addr,sizeof(addr))<0){ perror("connect"); close(sock); return; }
    std::string probe="HEAD / HTTP/1.0\r\n\r\n";
    send(sock,probe.c_str(),probe.size(),0);
    char buf[4096];
    int n=recv(sock,buf,sizeof(buf)-1,0);
    if(n>0){
        buf[n]=0;
        std::cout<<buf<<"\n";
    }
    close(sock);
}
}
