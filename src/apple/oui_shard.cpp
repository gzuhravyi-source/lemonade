#include "apple/oui_shard.h"
#include "layer2/mac.h"
#include <iostream>
void lemonade::OUIShard::map(const std::vector<std::string>& args){
    MAC::lookup(args); // reuse MAC lookup with enhanced sharding? Just call.
}
