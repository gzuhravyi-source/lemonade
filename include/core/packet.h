#ifndef LEMONADE_PACKET_H
#define LEMONADE_PACKET_H
#include <cstdint>
#include <vector>
#include <string>
namespace lemonade {
class Packet {
public:
    Packet();
    bool fromBytes(const std::vector<uint8_t>&);
    std::vector<uint8_t> toBytes() const;
    uint8_t* ethernetHeader();
    size_t size() const;
private:
    std::vector<uint8_t> raw;
};
}
#endif
