#ifndef RELATION_H
#define RELATION_H

#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>

#include "Fragment.h"
#include "Attribute.h"
#include "../utils/utils.h"

class Relation {
private:
    friend std::ostream& operator<<(std::ostream& os, Relation r);
public:
    std::string rname;
    bool is_horizontal;
    int num_of_recs;
    std::vector<Attribute> attributes;
    std::vector<Fragment> frags;

    Relation(std::string rname, bool is_horizontal=true, int num_of_recs=0);
    Relation(std::string rname, std::vector<Attribute> attributes, bool is_horizontal=true, int num_of_recs=0);
    void add_attribute(Attribute a);
    void add_attribute(std::string aname, bool is_key, int type);
    void add_attribute(std::string aname, bool is_key, int type, int value_type, std::vector<double> value);
    void add_attribute(std::string aname, bool is_key, int type, int value_type, std::map<std::string, double> proportion);
    void add_fragment(Fragment f);
    void add_fragment(std::string rname, std::string fname, std::string sname, bool is_horizontal, std::vector<std::string> vf_condition);
    void add_fragment(std::string rname, std::string fname, std::string sname, bool is_horizontal, std::vector<Predicate> hf_condition);
    void print_fragments();

    void set_num_of_recs(int n);    // set the rows of the relation table
    std::vector<std::string> get_anames();      // get global table attributes
    std::vector<std::string> get_attrs_meta();  // get global table attributes(meta data)
    std::vector<std::string> get_fragmented_attrs_meta(std::string sname);  // get fragmented table attributes
    bool in_site(std::string sname);  // Determine whether the relation table is assigned to the current site
    std::unordered_map<std::string, std::string> get_site_to_insert(std::string values);
    std::string get_key();
    int get_key_type();
};

#endif