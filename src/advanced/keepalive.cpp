#include <iostream>
#include <string>
#include <vector>
#include "core/utils.h"

#ifdef _WIN32
    #include <winsock2.h>
    #include <mstcpip.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <unistd.h>
#endif

namespace lemonade {
namespace KeepAlive {

void start(const std::string& ip, int port) {
#ifdef _WIN32
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#else
    int sock = socket(AF_INET, SOCK_STREAM, 0);
#endif

    if (sock < 0) return;

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = lemonade::ip_to_int(ip);

    // 1. Enable basic KeepAlive
    int optval = 1;
    setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (const char*)&optval, sizeof(optval));

    // 2. Set advanced parameters (System Dependent)
    int idle = 60;
    int count = 5;
    int intvl = 10;

#ifdef __APPLE__
    // macOS uses TCP_KEEPALIVE
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPALIVE, &idle, sizeof(idle));
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &count, sizeof(count));
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &intvl, sizeof(intvl));
#elif defined(_WIN32)
    // Windows uses SIO_KEEPALIVE_VALS
    tcp_keepalive alive;
    alive.onoff = 1;
    alive.keepalivetime = idle * 1000;
    alive.keepaliveinterval = intvl * 1000;
    DWORD bytesReturned;
    WSAIoctl(sock, SIO_KEEPALIVE_VALS, &alive, sizeof(alive), NULL, 0, &bytesReturned, NULL, NULL);
#else
    // Linux and others
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(idle));
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &count, sizeof(count));
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &intvl, sizeof(intvl));
#endif

    std::cout << "Cross-platform Keep-alive set for " << ip << ":" << port << std::endl;
}

}
}
