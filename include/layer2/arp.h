#ifndef LEMONADE_ARP_H
#define LEMONADE_ARP_H
#include <vector>
#include <string>
namespace lemonade { namespace ARP {
void poison(const std::vector<std::string>&);
void passive_sniff(const std::vector<std::string>&);
void gratuitous(const std::vector<std::string>&);
void flood(const std::vector<std::string>&);
}}
#endif
