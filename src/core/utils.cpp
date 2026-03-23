#include "core/utils.h"

namespace lemonade {

uint32_t ip_to_int(const std::string& ip) {
#ifdef _WIN32
    return inet_addr(ip.c_str());
#else
    struct in_addr addr;
    if (inet_aton(ip.c_str(), &addr) == 0) return 0;
    return addr.s_addr;
#endif
}

uint16_t calculate_checksum(unsigned short *ptr, int nbytes) {
    long sum = 0;
    unsigned short oddbyte;
    short answer;

    while (nbytes > 1) {
        sum += *ptr++;
        nbytes -= 2;
    }
    if (nbytes == 1) {
        oddbyte = 0;
        *((unsigned char *)&oddbyte) = *(unsigned char *)ptr;
        sum += oddbyte;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum = sum + (sum >> 16);
    answer = (short)~sum;

    return (answer);
}

}
