#ifndef LEMONADE_SOCKET_H
#define LEMONADE_SOCKET_H
#include <string>
#include <vector>
namespace lemonade {
class RawSocket {
public:
    RawSocket(const std::string& iface);
    ~RawSocket();
    bool open();
    void close();
    bool send(const std::vector<uint8_t>& data);
    std::vector<uint8_t> receive(int timeout_ms=1000);
    uint8_t* get_mac();
    int get_fd() const { return fd; }
private:
    int fd;
    std::string interface;
    uint8_t mac[6];
};
}
#endif
