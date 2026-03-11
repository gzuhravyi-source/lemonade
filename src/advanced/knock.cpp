#include "advanced/knock.h"
#include "layer3/tcp.h"
void lemonade::Knock::test(const std::vector<std::string>& args){
    TCP::knock_test(args);
}
