#include "advanced/hexedit.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>

namespace lemonade {
    void HexEdit::view(const std::vector<std::string>& args) {
        if(args.size() < 2) {
            std::cerr << "Usage: -hexedit <pcap_file> <packet_num>\n";
            return;
        }
        
        std::string file = args[0];
        int pkt_target = std::stoi(args[1]);
        std::ifstream f(file, std::ios::binary);
        
        f.seekg(24); // Skip global PCAP header
        int current_pkt = 0;
        
        while(!f.eof()) {
            uint32_t ts_sec, ts_usec, incl_len, orig_len;
            f.read((char*)&ts_sec, 4);
            f.read((char*)&ts_usec, 4);
            f.read((char*)&incl_len, 4);
            f.read((char*)&orig_len, 4);
            
            if(f.gcount() != 4) break;
            current_pkt++;
            
            if(current_pkt == pkt_target) {
                std::vector<uint8_t> data(incl_len);
                f.read((char*)data.data(), incl_len);
                for(size_t i = 0; i < data.size(); i++) {
                    if(i % 16 == 0) std::cout << "\n" << std::setw(4) << std::setfill('0') << std::hex << i << ": ";
                    std::cout << std::setw(2) << (int)data[i] << " ";
                }
                std::cout << std::dec << "\n";
                return;
            }
            f.seekg(incl_len, std::ios::cur);
        }
    }
}
