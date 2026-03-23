#ifndef LEMONADE_UTILS_H
#define LEMONADE_UTILS_H

#include <string>
#include <cstdint>

#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <arpa/inet.h>
#endif

namespace lemonade {
    // Converts IP string to integer (handles byte order)
    uint32_t ip_to_int(const std::string& ip);
    
    // Generic checksum for raw packet injection
    uint16_t calculate_checksum(unsigned short *ptr, int nbytes);
}

#endif
