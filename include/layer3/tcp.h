#ifndef LEMONADE_TCP_H
#define LEMONADE_TCP_H
#include <vector>
#include <string>
namespace lemonade { namespace TCP {
void syn_scan(const std::vector<std::string>&);
void window_fingerprint(const std::vector<std::string>&);
void xmas_scan(const std::vector<std::string>&);
void hijack_sim(const std::vector<std::string>&);
void keepalive_test(const std::vector<std::string>&);
void knock_test(const std::vector<std::string>&);
void udp_probe(const std::vector<std::string>&);
}}
#endif
