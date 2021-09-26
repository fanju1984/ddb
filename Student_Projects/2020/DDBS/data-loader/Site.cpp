#include "Site.h"

Site::Site(std::string sname, std::string ip, std::string port) :
sname(sname), ip(ip), port(port) {}

std::string Site::get_url() {
    return this->ip + std::string(":") + this->port;
}

std::ostream& operator<<(std::ostream& os, Site s) {
    os << s.sname << std::string(" ") << s.ip << std::string(":") << s.port;
    return os;
}
