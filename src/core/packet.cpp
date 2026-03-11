#include "core/packet.h"
namespace lemonade {
Packet::Packet() {}
bool Packet::fromBytes(const std::vector<uint8_t>& d) { raw=d; return true; }
std::vector<uint8_t> Packet::toBytes() const { return raw; }
uint8_t* Packet::ethernetHeader() { return raw.data(); }
size_t Packet::size() const { return raw.size(); }
}
