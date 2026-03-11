#include "layer3/tcp.h"
#include "core/utils.h"
#include <iostream>
#include <thread>
#include <vector>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <poll.h>
#include <map>
#include <chrono>
namespace lemonade {
struct tcphdr {
    uint16_t source;
    uint16_t dest;
    uint32_t seq;
    uint32_t ack_seq;
    uint16_t res1:4, doff:4, fin:1, syn:1, rst:1, psh:1, ack:1, urg:1, ece:1, cwr:1;
    uint16_t window;
    uint16_t check;
    uint16_t urg_ptr;
};
void TCP::syn_scan(const std::vector<std::string>& args) {
    if(args.size()<2){ std::cerr<<"Usage: -synscan <ip> <port_range>\n"; return; }
    std::string ip=args[0];
    std::string range=args[1];
    int start=1,end=1024;
    if(range.find('-')!=std::string::npos){
        sscanf(range.c_str(),"%d-%d",&start,&end);
    } else {
        start=end=std::stoi(range);
    }
    int sockfd=socket(AF_INET,SOCK_RAW,IPPROTO_TCP);
    if(sockfd<0){ perror("socket"); return; }
    int one=1;
    setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one));
    fcntl(sockfd,F_SETFL,fcntl(sockfd,F_GETFL,0)|O_NONBLOCK);
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=ip_to_int(ip);
    std::map<int,bool> open_ports;
    for(int port=start;port<=end;port++){
        char packet[sizeof(struct iphdr)+sizeof(struct tcphdr)];
        struct iphdr* ip=(struct iphdr*)packet;
        struct tcphdr* tcp=(struct tcphdr*)(packet+sizeof(struct iphdr));
        ip->version=4; ip->ihl=5;
        ip->tos=0;
        ip->tot_len=htons(sizeof(packet));
        ip->id=htons(rand()%65535);
        ip->frag_off=0;
        ip->ttl=64;
        ip->protocol=IPPROTO_TCP;
        ip->saddr=INADDR_ANY;
        ip->daddr=addr.sin_addr.s_addr;
        ip->check=0;
        tcp->source=htons(rand()%65535);
        tcp->dest=htons(port);
        tcp->seq=htonl(rand());
        tcp->ack_seq=0;
        tcp->doff=5;
        tcp->fin=0; tcp->syn=1; tcp->rst=0; tcp->psh=0; tcp->ack=0; tcp->urg=0;
        tcp->window=htons(1024);
        tcp->check=0;
        tcp->urg_ptr=0;
        ip->check=checksum((uint16_t*)ip, sizeof(struct iphdr));
        tcp->check=0; // pseudo header omitted for simplicity
        sendto(sockfd,packet,sizeof(packet),0,(struct sockaddr*)&addr,sizeof(addr));
        std::this_thread::sleep_for(std::chrono::microseconds(500));
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    // Receive responses
    while(true){
        struct sockaddr_in sender;
        socklen_t senderlen=sizeof(sender);
        char buf[1500];
        int n=recvfrom(sockfd,buf,sizeof(buf),0,(struct sockaddr*)&sender,&senderlen);
        if(n<=0) break;
        struct iphdr* ip=(struct iphdr*)buf;
        if(ip->protocol!=IPPROTO_TCP) continue;
        int iphdrlen=ip->ihl*4;
        struct tcphdr* tcp=(struct tcphdr*)(buf+iphdrlen);
        if(tcp->syn==1 && tcp->ack==1){
            open_ports[ntohs(tcp->source)]=true;
        }
    }
    close(sockfd);
    for(auto& p:open_ports) std::cout<<"Port "<<p.first<<" open\n";
}
void TCP::window_fingerprint(const std::vector<std::string>& args) {
    if(args.size()<2){ std::cerr<<"Usage: -tcpwindow <ip> <port>\n"; return; }
    std::string ip=args[0];
    int port=std::stoi(args[1]);
    int sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd<0){ perror("socket"); return; }
    fcntl(sockfd,F_SETFL,fcntl(sockfd,F_GETFL,0)|O_NONBLOCK);
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=ip_to_int(ip);
    addr.sin_port=htons(port);
    connect(sockfd,(struct sockaddr*)&addr,sizeof(addr));
    struct pollfd pfd={sockfd,POLLOUT,0};
    poll(&pfd,1,1000);
    int error=0;
    socklen_t len=sizeof(error);
    getsockopt(sockfd,SOL_SOCKET,SO_ERROR,&error,&len);
    if(error==0){
        // connected, get window size via getsockopt? Not directly. Use raw socket to capture.
        std::cout<<"Window fingerprint: check manually\n";
    }
    close(sockfd);
}
void TCP::xmas_scan(const std::vector<std::string>& args) {
    if(args.size()<2){ std::cerr<<"Usage: -xmas <ip> <port>\n"; return; }
    std::string ip=args[0];
    int port=std::stoi(args[1]);
    int sockfd=socket(AF_INET,SOCK_RAW,IPPROTO_TCP);
    if(sockfd<0){ perror("socket"); return; }
    int one=1;
    setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one));
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=ip_to_int(ip);
    char packet[sizeof(struct iphdr)+sizeof(struct tcphdr)];
    struct iphdr* ip=(struct iphdr*)packet;
    struct tcphdr* tcp=(struct tcphdr*)(packet+sizeof(struct iphdr));
    ip->version=4; ip->ihl=5;
    ip->tos=0;
    ip->tot_len=htons(sizeof(packet));
    ip->id=htons(rand()%65535);
    ip->frag_off=0;
    ip->ttl=64;
    ip->protocol=IPPROTO_TCP;
    ip->saddr=INADDR_ANY;
    ip->daddr=addr.sin_addr.s_addr;
    ip->check=0;
    tcp->source=htons(rand()%65535);
    tcp->dest=htons(port);
    tcp->seq=htonl(rand());
    tcp->ack_seq=0;
    tcp->doff=5;
    tcp->fin=1; tcp->syn=1; tcp->rst=1; tcp->psh=1; tcp->ack=1; tcp->urg=1;
    tcp->window=htons(1024);
    tcp->check=0;
    tcp->urg_ptr=0;
    ip->check=checksum((uint16_t*)ip, sizeof(struct iphdr));
    tcp->check=0; // pseudo header omitted
    sendto(sockfd,packet,sizeof(packet),0,(struct sockaddr*)&addr,sizeof(addr));
    close(sockfd);
    std::cout<<"XMAS packet sent\n";
}
void TCP::hijack_sim(const std::vector<std::string>& args) {
    std::cout<<"TCP hijack sim not implemented\n";
}
void TCP::keepalive_test(const std::vector<std::string>& args) {
    std::cout<<"Keepalive test not implemented\n";
}
void TCP::knock_test(const std::vector<std::string>& args) {
    if(args.size()<2){ std::cerr<<"Usage: -knock <ip> <port1,port2,port3>\n"; return; }
    std::string ip=args[0];
    std::string ports_str=args[1];
    std::vector<int> ports;
    size_t pos=0;
    while((pos=ports_str.find(','))!=std::string::npos){
        ports.push_back(std::stoi(ports_str.substr(0,pos)));
        ports_str.erase(0,pos+1);
    }
    ports.push_back(std::stoi(ports_str));
    for(int port:ports){
        int sock=socket(AF_INET,SOCK_STREAM,0);
        struct sockaddr_in addr;
        addr.sin_family=AF_INET;
        addr.sin_addr.s_addr=ip_to_int(ip);
        addr.sin_port=htons(port);
        fcntl(sock,F_SETFL,fcntl(sock,F_GETFL,0)|O_NONBLOCK);
        connect(sock,(struct sockaddr*)&addr,sizeof(addr));
        close(sock);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    std::cout<<"Knock sequence sent\n";
}
void TCP::udp_probe(const std::vector<std::string>& args) {
    if(args.size()<2){ std::cerr<<"Usage: -udpprobe <ip> <port_range>\n"; return; }
    std::string ip=args[0];
    std::string range=args[1];
    int start=1,end=1024;
    if(range.find('-')!=std::string::npos){
        sscanf(range.c_str(),"%d-%d",&start,&end);
    } else {
        start=end=std::stoi(range);
    }
    int sockfd=socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd<0){ perror("socket"); return; }
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=ip_to_int(ip);
    fcntl(sockfd,F_SETFL,fcntl(sockfd,F_GETFL,0)|O_NONBLOCK);
    for(int port=start;port<=end;port++){
        addr.sin_port=htons(port);
        sendto(sockfd,"",0,0,(struct sockaddr*)&addr,sizeof(addr));
        std::this_thread::sleep_for(std::chrono::microseconds(500));
    }
    // Need to listen for ICMP port unreachable; for simplicity, just print
    std::cout<<"UDP probes sent\n";
    close(sockfd);
}
}
