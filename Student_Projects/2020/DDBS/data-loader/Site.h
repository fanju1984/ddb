#ifndef SITE_H
#define SITE_H

#include <string>
#include <vector>

class Site {
private:
    friend std::ostream& operator<<(std::ostream& os, Site s);
public:
    std::string sname;
    std::string ip;
    std::string port;

    Site(std::string sname, std::string ip, std::string port);
    std::string get_url();
};

#endif