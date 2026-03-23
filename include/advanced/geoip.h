#ifndef LEMONADE_GEOIP_H
#define LEMONADE_GEOIP_H

#include <string>
#include <vector>
#include <map>
#include <cstdint>

namespace lemonade {
namespace GeoIP {

extern std::map<uint32_t, std::pair<uint32_t, std::string>> geo_db;

void load_geo();
std::string lookup(const std::string& ip_str);

}
}

#endif
