#include "layer3/ip.h"
#include "core/socket.h"
#include "core/utils.h"
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <thread>
#include <chrono>
#include <vector>
#include <map>
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <fcntl.h>
namespace lemonade {
struct iphdr {
    uint8_t ihl:4, version:4;
    uint8_t tos;
    uint16_t tot_len;
    uint16_t id;
    uint16_t frag_off;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t check;
    uint32_t saddr;
    uint32_t daddr;
};
void IP::id_analyze(const std::vector<std::string>& args) {
    if(args.size()<1){ std::cerr<<"Usage: -ipid <ip>\n"; return; }
    std::string target=args[0];
    int sockfd=socket(AF_INET,SOCK_RAW,IPPROTO_RAW);
    if(sockfd<0){ perror("socket"); return; }
    int one=1;
    if(setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one))<0){ perror("setsockopt"); close(sockfd); return; }
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=ip_to_int(target);
    std::vector<int> ids;
    for(int i=0;i<10;i++){
        char packet[sizeof(iphdr)];
        iphdr* ip=(iphdr*)packet;
        ip->version=4; ip->ihl=5;
        ip->tos=0;
        ip->tot_len=htons(sizeof(iphdr));
        ip->id=htons(rand()%65536);
        ip->frag_off=0;
        ip->ttl=64;
        ip->protocol=IPPROTO_ICMP;
        ip->saddr=INADDR_ANY;
        ip->daddr=addr.sin_addr.s_addr;
        ip->check=0;
        ip->check=checksum((uint16_t*)ip, sizeof(iphdr));
        sendto(sockfd, packet, sizeof(iphdr), 0, (struct sockaddr*)&addr, sizeof(addr));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    close(sockfd);
    std::cout<<"IP ID analysis complete\n";
}
void IP::fragment_test(const std::vector<std::string>& args) {
    std::cout<<"Fragment test not implemented\n";
}
void IP::source_route(const std::vector<std::string>& args) {
    std::cout<<"Source route test not implemented\n";
}
void IP::mtu_discover(const std::vector<std::string>& args) {
    if(args.size()<2){ std::cerr<<"Usage: -mtu <ip> <start_size>\n"; return; }
    std::string target=args[0];
    int size=std::stoi(args[1]);
    int sockfd=socket(AF_INET,SOCK_RAW,IPPROTO_ICMP);
    if(sockfd<0){ perror("socket"); return; }
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=ip_to_int(target);
    int flags=fcntl(sockfd,F_GETFL,0);
    fcntl(sockfd,F_SETFL,flags|O_NONBLOCK);
    for(int mtu=size;mtu>0;mtu-=8){
        char packet[mtu];
        memset(packet,0,mtu);
        struct icmp* icmp=(struct icmp*)packet;
        icmp->icmp_type=ICMP_ECHO;
        icmp->icmp_code=0;
        icmp->icmp_id=getpid();
        icmp->icmp_seq=1;
        icmp->icmp_cksum=0;
        icmp->icmp_cksum=checksum((uint16_t*)icmp,mtu);
        sendto(sockfd,packet,mtu,0,(struct sockaddr*)&addr,sizeof(addr));
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(sockfd,&fds);
        struct timeval tv={1,0};
        if(select(sockfd+1,&fds,NULL,NULL,&tv)>0){
            char recvbuf[1500];
            socklen_t addrlen=sizeof(addr);
            int n=recvfrom(sockfd,recvbuf,sizeof(recvbuf),0,(struct sockaddr*)&addr,&addrlen);
            if(n>0){
                struct ip* ip=(struct ip*)recvbuf;
                if(ip->ip_p==IPPROTO_ICMP){
                    struct icmp* icmp_resp=(struct icmp*)(recvbuf+ip->ip_hl*4);
                    if(icmp_resp->icmp_type==ICMP_ECHOREPLY){
                        std::cout<<"MTU works: "<<mtu<<"\n";
                        break;
                    }
                }
            }
        }
    }
    close(sockfd);
}
void IP::traceroute(const std::vector<std::string>& args) {
    if(args.size()<2){ std::cerr<<"Usage: -traceroute <ip> <max_ttl>\n"; return; }
    std::string target=args[0];
    int max_ttl=std::stoi(args[1]);
    int sockfd=socket(AF_INET,SOCK_RAW,IPPROTO_ICMP);
    if(sockfd<0){ perror("socket"); return; }
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=ip_to_int(target);
    for(int ttl=1;ttl<=max_ttl;ttl++){
        int ttl_val=ttl;
        setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl_val, sizeof(ttl_val));
        char packet[64];
        struct icmp* icmp=(struct icmp*)packet;
        icmp->icmp_type=ICMP_ECHO;
        icmp->icmp_code=0;
        icmp->icmp_id=getpid();
        icmp->icmp_seq=ttl;
        icmp->icmp_cksum=0;
        icmp->icmp_cksum=checksum((uint16_t*)icmp,64);
        sendto(sockfd,packet,64,0,(struct sockaddr*)&addr,sizeof(addr));
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(sockfd,&fds);
        struct timeval tv={2,0};
        if(select(sockfd+1,&fds,NULL,NULL,&tv)>0){
            char recvbuf[1500];
            socklen_t addrlen=sizeof(addr);
            int n=recvfrom(sockfd,recvbuf,sizeof(recvbuf),0,(struct sockaddr*)&addr,&addrlen);
            if(n>0){
                struct ip* ip=(struct ip*)recvbuf;
                struct icmp* icmp_resp=(struct icmp*)(recvbuf+ip->ip_hl*4);
                if(icmp_resp->icmp_type==ICMP_TIME_EXCEEDED){
                    struct ip* orig_ip=(struct ip*)(icmp_resp+1);
                    std::cout<<"TTL "<<ttl<<": "<<inet_ntoa(ip->ip_src)<<"\n";
                } else if(icmp_resp->icmp_type==ICMP_ECHOREPLY){
                    std::cout<<"Reached target at TTL "<<ttl<<"\n";
                    break;
                }
            }
        } else {
            std::cout<<"TTL "<<ttl<<": *\n";
        }
    }
    close(sockfd);
}
}
