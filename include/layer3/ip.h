#ifndef LEMONADE_IP_H
#define LEMONADE_IP_H
#include <vector>
#include <string>
namespace lemonade { namespace IP {
void id_analyze(const std::vector<std::string>&);
void fragment_test(const std::vector<std::string>&);
void source_route(const std::vector<std::string>&);
void mtu_discover(const std::vector<std::string>&);
void traceroute(const std::vector<std::string>&);
}}
#endif
