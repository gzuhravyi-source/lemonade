#ifndef LEMONADE_SOCKET_H
#define LEMONADE_SOCKET_H

#include <string>
#include <vector>
#include <unistd.h> 

namespace lemonade {
class RawSocket {
public:
    RawSocket(const std::string& iface) : fd(-1), interface(iface) {}
    
    
    ~RawSocket() { close(); }

    RawSocket(const RawSocket&) = delete;
    RawSocket& operator=(const RawSocket&) = delete;

    bool open();
    void close() {
        if (fd != -1) {
            ::close(fd);
            fd = -1;
        }
    }

    bool send(const std::vector<uint8_t>& data);
    std::vector<uint8_t> receive(int timeout_ms = 1000);
    uint8_t* get_mac();
    int get_fd() const { return fd; }

private:
    int fd;
    std::string interface;
    uint8_t mac[6];
};
}
#endif
