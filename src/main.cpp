#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include "layer2/arp.h"
#include "layer2/mac.h"
#include "layer2/vlan.h"
#include "layer2/stp.h"
#include "layer2/ndp.h"
#include "layer3/ip.h"
#include "layer3/icmp.h"
#include "layer3/tcp.h"
#include "layer3/udp.h"
#include "layer7/http.h"
#include "layer7/dns.h"
#include "layer7/ssl.h"
#include "layer7/snmp.h"
#include "layer7/smb.h"
#include "layer7/dhcp.h"
#include "layer7/ftp.h"
#include "layer7/upnp.h"
#include "perf/jitter.h"
#include "perf/throughput.h"
#include "perf/latency.h"
#include "perf/entropy.h"
#include "forensic/pcap.h"
#include "forensic/flow.h"
#include "forensic/beacon.h"
#include "forensic/bogon.h"
#include "advanced/mdns.h"
#include "advanced/llmnr.h"
#include "advanced/p2p.h"
#include "advanced/torcheck.h"
#include "advanced/rdns.h"
#include "advanced/hijack.h"
#include "advanced/whois.h"
#include "advanced/geoip.h"
#include "advanced/socket_exhaust.h"
#include "advanced/hexedit.h"
#include "advanced/keepalive.h"
#include "advanced/knock.h"
#include "apple/airplay.h"
#include "apple/awdl.h"
#include "apple/continuity.h"
#include "apple/darkwake.h"
#include "apple/icloudproxy.h"
#include "apple/universal_control.h"
#include "apple/handoff.h"
#include "apple/darwin_icmp.h"
#include "apple/homekit.h"
#include "apple/oui_shard.h"

using namespace lemonade;

int main(int argc, char* argv[]) {
    if(argc < 2){
        std::cerr<<"Usage: lemonade -<flag> [args]\n";
        return 1;
    }
    std::string flag=argv[1];
    std::vector<std::string> args;
    for(int i=2;i<argc;i++) args.push_back(argv[i]);

    if(flag=="-arppoison") ARP::poison(args);
    else if(flag=="-arp" && args.size()>=1 && args[0]=="sniff") {
        std::vector<std::string> sub(args.begin()+1, args.end());
        ARP::passive_sniff(sub);
    }
    else if(flag=="-arp" && args.size()>=1 && args[0]=="gratuitous") {
        std::vector<std::string> sub(args.begin()+1, args.end());
        ARP::gratuitous(sub);
    }
    else if(flag=="-maclookup") MAC::lookup(args);
    else if(flag=="-macflood") ARP::flood(args);
    else if(flag=="-vlaninject") VLAN::inject(args);
    else if(flag=="-stpmap") STP::map(args);
    else if(flag=="-ndpscan") NDP::scan(args);
    else if(flag=="-ipid") IP::id_analyze(args);
    else if(flag=="-fragtest") IP::fragment_test(args);
    else if(flag=="-sourceroute") IP::source_route(args);
    else if(flag=="-mtu") IP::mtu_discover(args);
    else if(flag=="-traceroute") IP::traceroute(args);
    else if(flag=="-icmptimestamp") ICMP::timestamp(args);
    else if(flag=="-synscan") TCP::syn_scan(args);
    else if(flag=="-tcpwindow") TCP::window_fingerprint(args);
    else if(flag=="-xmas") TCP::xmas_scan(args);
    else if(flag=="-tcphijack") TCP::hijack_sim(args);
    else if(flag=="-keepalive") TCP::keepalive_test(args);
    else if(flag=="-knock") TCP::knock_test(args);
    else if(flag=="-udpprobe") TCP::udp_probe(args);
    else if(flag=="-httpfuzz") HTTP::fuzz(args);
    else if(flag=="-banner") HTTP::banner_grab(args);
    else if(flag=="-dnsrec") DNS::recursive_check(args);
    else if(flag=="-sslparse") SSL::parse_cert(args);
    else if(flag=="-snmpbrute") SNMP::brute(args);
    else if(flag=="-smbver") SMB::version_detect(args);
    else if(flag=="-dhcpstarve") DHCP::starvation(args);
    else if(flag=="-ftpbounce") FTP::bounce_scan(args);
    else if(flag=="-upnpdisc") UPNP::discover(args);
    else if(flag=="-jitter") Jitter::calculate(args);
    else if(flag=="-thruput") Throughput::saturate(args);
    else if(flag=="-latency") Latency::profile(args);
    else if(flag=="-entropy") Entropy::calculate(args);
    else if(flag=="-pcapread") PCAP::read(args);
    else if(flag=="-flow") Flow::analyze(args);
    else if(flag=="-beacon") Beacon::detect(args);
    else if(flag=="-bogon") Bogon::check(args);
    else if(flag=="-honey") std::cout<<"Honey-client not implemented\n";
    else if(flag=="-mdnsrespond") MDNS::respond(args);
    else if(flag=="-llmnrspoof") LLMNR::spoof(args);
    else if(flag=="-p2pfind") P2P::find_nodes(args);
    else if(flag=="-torcheck") TorCheck::check(args);
    else if(flag=="-rdns") RDNS::batch(args);
    else if(flag=="-whois") Whois::lookup(args);
    else if(flag=="-geoip") GeoIP::map(args);
    else if(flag=="-sockexhaust") SocketExhaust::test(args);
    else if(flag=="-hexedit") HexEdit::view(args);
    else if(flag=="-airplay") AirPlay::probe(args);
    else if(flag=="-awdlsniff") AWDL::sniff(args);
    else if(flag=="-continuity") Continuity::identify(args);
    else if(flag=="-darkwake") DarkWake::arp_nudge(args);
    else if(flag=="-icloudproxy") ICloudProxy::query(args);
    else if(flag=="-universalctl") UniversalControl::fingerprint(args);
    else if(flag=="-handoff") Handoff::trace(args);
    else if(flag=="-darwinicmp") DarwinICMP::analyze(args);
    else if(flag=="-homekit") HomeKit::probe(args);
    else if(flag=="-ouishard") OUIShard::map(args);
    else std::cerr<<"Unknown flag: "<<flag<<"\n";

    return 0;
}
