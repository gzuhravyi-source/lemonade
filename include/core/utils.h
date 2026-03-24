#ifndef LEMONADE_UTILS_H
#define LEMONADE_UTILS_H

#include <string>
#include <vector>
#include <cstdint>

namespace lemonade {
    uint32_t ip_to_int(const std::string&);
    std::string int_to_ip(uint32_t);
    std::string mac_to_string(const uint8_t* mac);
    bool string_to_mac(const std::string& str, uint8_t* mac);
    uint16_t checksum(uint16_t* buf, int len);
}
#endif
