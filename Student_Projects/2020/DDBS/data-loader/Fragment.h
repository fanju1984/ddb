#ifndef FRAGMENT_H
#define FRAGMENT_H

#include <vector>
#include <string>

#include "Predicate.h"
#include "../utils/utils.h"

class Fragment {
private:
    friend std::ostream& operator<<(std::ostream& os, Fragment f);
public:
    std::string rname; // relation name
    std::string fname; // e.g.cus1 cus2 ord1 ord2 ord3 ord4 pub1 ...
    std::string sname; // site name
    bool is_horizontal;
    int num_of_recs;
    std::vector<std::string> vf_condition; //e.g. customer_id customer_name
    std::vector<Predicate> hf_condition;

    Fragment(std::string rname, std::string fname, std::string sname, bool is_horizontal, std::vector<std::string> vf_condition);
    Fragment(std::string rname, std::string fname, std::string sname, bool is_horizontal, std::vector<Predicate> hf_condition);
    Fragment(std::string rname, std::string fname, bool is_horizontal, std::vector<std::string> vf_condition);
    Fragment(std::string rname, std::string fname, bool is_horizontal, std::vector<Predicate> hf_condition);

    void set_num_of_recs(int n);
    void set_sname(std::string sname);
};

#endif