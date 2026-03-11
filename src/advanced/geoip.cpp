    std::ifstream f("data/geoip.csv");
    std::string line;
    while(std::getline(f,line)){
        size_t pos1=line.find(',');
        if(pos1==std::string::npos) continue;
        size_t pos2=line.find(',',pos1+1);
        if(pos2==std::string::npos) continue;
        uint32_t start=ip_to_int(line.substr(0,pos1));
        uint32_t end=ip_to_int(line.substr(pos1+1,pos2-pos1-1));
        std::string country=line.substr(pos2+1);
        geo_db[start]=std::make_pair(end,country);
    }
}
void GeoIP::map(const std::vector<std::string>& args) {
    if(args.size()<1){ std::cerr<<"Usage: -geoip <ip>\n"; return; }
    std::string ip=args[0];
    if(geo_db.empty()) load_geo();
    uint32_t ip_int=ip_to_int(ip);
    auto it=geo_db.upper_bound(ip_int);
    if(it!=geo_db.begin()) --it;
    if(it!=geo_db.end() && ip_int>=it->first && ip_int<=it->second.first){
        std::cout<<"Country: "<<it->second.second<<"\n";
    } else {
        std::cout<<"Unknown location\n";
    }
}
}
