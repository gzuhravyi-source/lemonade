#include "layer3/udp.h"
#include "layer3/tcp.h" // reuse TCP::udp_probe
namespace lemonade {
void UDP::probe(const std::vector<std::string>& args){
    TCP::udp_probe(args);
}
}
