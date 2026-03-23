#include "advanced/hexedit.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>

namespace lemonade { 
namespace HexEdit {

void view(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cerr << "Usage: -hexedit <pcap_file> <packet_num>\n";
        return;
    }

    std::string filename = args[0];
    int target_pkt = std::stoi(args[1]);
    std::ifstream f(filename, std::ios::binary);

    if (!f.is_open()) {
        std::cerr << "Error: Could not open " << filename << "\n";
        return;
    }

    // Skip PCAP global header (24 bytes) [cite: 65]
    f.seekg(24, std::ios::beg);

    int current_pkt = 0;
    while (f) {
        uint32_t ts_sec, ts_usec, incl_len, orig_len;
        
        // Read packet header [cite: 66]
        if (!f.read(reinterpret_cast<char*>(&ts_sec), 4)) break;
        if (!f.read(reinterpret_cast<char*>(&ts_usec), 4)) break;
        if (!f.read(reinterpret_cast<char*>(&incl_len), 4)) break;
        if (!f.read(reinterpret_cast<char*>(&orig_len), 4)) break;

        current_pkt++;

        if (current_pkt == target_pkt) {
            std::vector<uint8_t> data(incl_len);
            f.read(reinterpret_cast<char*>(data.data()), incl_len);

            std::cout << "Packet #" << target_pkt << " (" << incl_len << " bytes):";
            for (size_t i = 0; i < data.size(); i++) {
                if (i % 16 == 0) std::cout << "\n" << std::hex << std::setw(4) << std::setfill('0') << i << ": ";
                std::cout << std::hex << std::setw(2) << static_cast<int>(data[i]) << " ";
            }
            std::cout << std::dec << "\n";
            return;
        } else {
            // Skip the packet data to move to the next header
            f.seekg(incl_len, std::ios::cur);
        }
    }
    std::cerr << "Error: Packet number " << target_pkt << " not found.\n";
}
}}
