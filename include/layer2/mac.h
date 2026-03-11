#ifndef LEMONADE_MAC_H
#define LEMONADE_MAC_H
#include <string>
#include <vector>
#include <cstdint>
namespace lemonade { namespace MAC {
void lookup(const std::vector<std::string>&);
bool resolve(const std::string& ip, uint8_t* mac, const std::string& iface, int timeout_ms=2000);
}}
#endif
