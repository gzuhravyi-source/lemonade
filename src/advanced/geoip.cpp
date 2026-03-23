#include "advanced/geoip.h"
#include "core/utils.h"
#include <fstream>
#include <iostream>
#include <algorithm>

namespace lemonade {
namespace GeoIP {

// Helper to trim whitespace/newlines
std::string trim(const std::string& s) {
    size_t first = s.find_first_not_of(" \r\n\t");
    if (std::string::npos == first) return s;
    size_t last = s.find_last_not_of(" \r\n\t");
    return s.substr(first, (last - first + 1));
}

void load_geo() {
    std::ifstream f("data/geoip.csv");
    if (!f.is_open()) return;

    std::string line;
    while (std::getline(f, line)) {
        size_t pos1 = line.find(',');
        if (pos1 == std::string::npos) continue;

        size_t pos2 = line.find(',', pos1 + 1);
        if (pos2 == std::string::npos) continue;

        uint32_t start = ip_to_int(trim(line.substr(0, pos1)));
        uint32_t end = ip_to_int(trim(line.substr(pos1 + 1, pos2 - pos1 - 1)));
        std::string country = trim(line.substr(pos2 + 1));

        geo_db[start] = std::make_pair(end, country); // [cite: 61]
    }
}
}}
