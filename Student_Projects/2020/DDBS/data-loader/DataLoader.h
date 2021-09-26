#ifndef DATALOADER_H
#define DATALOADER_H

#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <unordered_map>
#include <fstream>

#include "Site.h"
#include "Relation.h"
#include "../utils/utils.h"
#include "../network-utils/network.h"

class DataLoader {
private:
    std::string sep = "/";
    std::map<std::string, std::string> key_map;
public:
    std::vector<Relation> relations;
    std::vector<Site> sites;
    std::unordered_map<std::string, Fragment> unallocated_fragments;

    DataLoader();

    void init();    // initial the relations, sites variables.

    void show_sites();
    void get_sites_from_etcd();
    void add_site(std::string sname, std::string ip, std::string port);
    int read_site_num_from_etcd();

    int read_relation_num_from_etcd();
    void get_relations_from_etcd();
    std::string get_prefix_by_rname(std::string rname);
    void show_tables(bool show_fragment=true);
    void add_relation(std::string rname, std::vector<Attribute> attributes);

    void add_unallocated_fragment(Fragment fragment);
    void allocate(std::string fname, std::string sname);
    void add_fragment(Fragment fragment);
    void show_unallocated_fragments();

    std::unordered_map<std::string, std::string> get_site_to_insert(std::string rname, std::string values);
    std::vector<Site> get_site_to_delete(std::string rname);

    Site get_site_by_sname(const std::string& sname);
    Relation get_relation_by_rname(std::string rname);
    std::vector<std::vector<std::string>> get_data_from_tsv(std::string file_path);
    std::map<std::string, std::vector<std::vector<std::string>>> fragment_data(std::string rname, std::vector<std::vector<std::string>> datas);
    std::vector<std::string> get_insert_values(std::string rname, std::vector<std::vector<std::string>> data);
    std::map<std::string, std::string> get_sites_map();
};

#endif