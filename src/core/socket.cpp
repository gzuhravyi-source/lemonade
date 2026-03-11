#include "core/socket.h"
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <poll.h>
#include <vector>
namespace lemonade {
RawSocket::RawSocket(const std::string& iface) : fd(-1), interface(iface) { memset(mac,0,6); }
RawSocket::~RawSocket() { if(fd!=-1) close(); }
bool RawSocket::open() {
    fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if(fd<0) return false;
    struct ifreq ifr;
    memset(&ifr,0,sizeof(ifr));
    strncpy(ifr.ifr_name, interface.c_str(), IFNAMSIZ-1);
    if(ioctl(fd, SIOCGIFHWADDR, &ifr)<0) return false;
    memcpy(mac, ifr.ifr_hwaddr.sa_data, 6);
    return true;
}
void RawSocket::close() { ::close(fd); fd=-1; }
bool RawSocket::send(const std::vector<uint8_t>& data) {
    struct sockaddr_ll addr;
    memset(&addr,0,sizeof(addr));
    addr.sll_family = AF_PACKET;
    addr.sll_ifindex = if_nametoindex(interface.c_str());
    addr.sll_protocol = htons(ETH_P_ALL);
    return ::sendto(fd, data.data(), data.size(), 0, (struct sockaddr*)&addr, sizeof(addr)) > 0;
}
std::vector<uint8_t> RawSocket::receive(int timeout_ms) {
    struct pollfd pfd; pfd.fd=fd; pfd.events=POLLIN;
    if(poll(&pfd,1,timeout_ms)<=0) return {};
    std::vector<uint8_t> buf(65536);
    ssize_t n = recvfrom(fd, buf.data(), buf.size(), 0, nullptr, nullptr);
    if(n<=0) return {};
    buf.resize(n);
    return buf;
}
uint8_t* RawSocket::get_mac() { return mac; }
}
