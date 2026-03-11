#include "layer7/ftp.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
namespace lemonade {
void FTP::bounce_scan(const std::vector<std::string>& args) {
    if(args.size()<3){ std::cerr<<"Usage: -ftpbounce <ftp_server> <target_ip> <port>\n"; return; }
    std::string ftp=args[0];
    std::string target=args[1];
    int port=std::stoi(args[2]);
    int sock=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=ip_to_int(ftp);
    addr.sin_port=htons(21);
    if(connect(sock,(struct sockaddr*)&addr,sizeof(addr))<0){ perror("connect"); close(sock); return; }
    char buf[256];
    recv(sock,buf,sizeof(buf),0);
    std::string user="USER anonymous\r\n";
    send(sock,user.c_str(),user.size(),0);
    recv(sock,buf,sizeof(buf),0);
    std::string pass="PASS test@\r\n";
    send(sock,pass.c_str(),pass.size(),0);
    recv(sock,buf,sizeof(buf),0);
    std::string portcmd="PORT ";
    struct in_addr in;
    in.s_addr=ip_to_int(target);
    unsigned char* p=(unsigned char*)&in;
    char portstr[100];
    sprintf(portstr,"%d,%d,%d,%d,%d,%d\r\n", p[0],p[1],p[2],p[3], (port>>8)&0xFF, port&0xFF);
    portcmd+=portstr;
    send(sock,portcmd.c_str(),portcmd.size(),0);
    recv(sock,buf,sizeof(buf),0);
    std::string list="LIST\r\n";
    send(sock,list.c_str(),list.size(),0);
    recv(sock,buf,sizeof(buf),0);
    std::cout<<"Bounce scan attempted\n";
    close(sock);
}
}
