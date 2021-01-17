#include "DataLoader.h"

DataLoader::DataLoader() {
    this->key_map.insert(std::pair<std::string, std::string>("ddbs", "ddbs"));
    this->key_map.insert(std::pair<std::string, std::string>("site_num", "ddbs" + this->sep + "nums_of_sites"));
    this->key_map.insert(std::pair<std::string, std::string>("sites", "ddbs" + this->sep + "sites"));
    this->key_map.insert(std::pair<std::string, std::string>("relation_num", "ddbs" + this->sep + "nums_of_relations"));
    this->key_map.insert(std::pair<std::string, std::string>("relations", "ddbs" + this->sep + "relations"));
    
    this->get_sites_from_etcd();
    this->get_relations_from_etcd();
}

void DataLoader::init() {
    if(delete_from_etcd_by_prefix(this->key_map["ddbs"]) == 0) {
        this->sites.clear(); 
        this->relations.clear();
        std::cout << "Init successfully.\n" << std::endl;
    } else {
        std::cout << "Init failed.\n" << std::endl;
    }
}

void DataLoader::show_tables(bool show_fragment) {
    for(auto relation : this->relations) {
        std::cout << relation;
        if(show_fragment) {
            relation.print_fragments();
        }
    }
    std::cout << std::endl;
}

std::map<std::string, std::vector<std::vector<std::string>>> DataLoader::fragment_data(std::string rname, std::vector<std::vector<std::string>> datas) {
    std::map<std::string, std::vector<std::vector<std::string>>> fragment_data_map;
    Relation relation = this->get_relation_by_rname(rname);
    if(relation.is_horizontal) {
        for(auto data : datas) {
            for(auto fragment : relation.frags) {
                std::vector<bool> result;
                for(auto predicate : fragment.hf_condition) {
                    for(int i=0; i<relation.attributes.size(); ++i) {
                        if(relation.attributes[i].aname == predicate.aname) {
                            switch (predicate.op_type) {
                                case 1:
                                    if(atof(data[i].c_str()) >= predicate.num) {
                                        result.push_back(true);
                                    }
                                    break;
                                case 2:
                                    if(atof(data[i].c_str()) <= predicate.num) {
                                        result.push_back(true);
                                    }
                                    break;
                                case 3:
                                    if(atof(data[i].c_str()) > predicate.num) {
                                        result.push_back(true);
                                    }
                                    break;
                                case 4:
                                    if(atof(data[i].c_str()) < predicate.num) {
                                        result.push_back(true);
                                    }
                                    break;
                                case 5:
                                    if(atof(data[i].c_str()) == predicate.num) {
                                        result.push_back(true);
                                    }
                                    break;
                                case 6:
                                    if(data[i] == predicate.str) {
                                        result.push_back(true);
                                    }
                                    break;
                                case 7:
                                    break; 
                                default:
                                    break;
                            }
                        }
                    }
                }
                if(result.size() == fragment.hf_condition.size()) {
                    fragment_data_map[fragment.sname].push_back(data);
                }
            }
        }
    } else {
        for(auto data : datas) {
            for(auto fragment : relation.frags) {
                std::vector<std::string> tmp;
                for(auto aname : fragment.vf_condition) {
                    for(int i=0; i<relation.attributes.size(); ++i) {
                        if(relation.attributes[i].aname == aname) {
                            tmp.push_back(data[i]);
                        }
                    }
                }
                fragment_data_map[fragment.sname].push_back(tmp);
            }
        }
    }
    return fragment_data_map;
}

std::vector<std::vector<std::string>> DataLoader::get_data_from_tsv(std::string file_path) {
    std::string file_error = std::string("Error opening file. Please check your filename.");
    std::ifstream fin(file_path, std::ios_base::in);
    std::string str;
    std::vector<std::vector<std::string>> res;
    if (fin.is_open()) {
        while(std::getline(fin, str)) {
            // split string with "\t"
            std::vector<std::string> v_str;
            split_string(str, v_str, std::string("\t"));
            res.push_back(v_str);
        }
    } else {
        std::cout << file_error << std::endl;
    }
    return res;
}

std::vector<std::string> DataLoader::get_insert_values(std::string rname, std::vector<std::vector<std::string>> data) {
    std::vector<std::string> res;
    Relation r = this->get_relation_by_rname(rname);
    if(data.size() == 0) return res;
    for(int i=0; i<data.size(); ++i) {
        std::string values = "";
        for(int j=0; j<data[i].size(); ++j) {
            if(j > 0) {
                values += std::string(", ");
            }
            if(r.attributes[j].type == 1) {
                values += data[i][j];
            } else if(r.attributes[j].type == 2) {
                values += std::string("'");
                values += data[i][j];
                values += std::string("'");
            }
        }
        res.push_back(values);
    }
    return res;
}

Relation DataLoader::get_relation_by_rname(std::string rname) {
    for(auto relation : this->relations) {
        if(relation.rname == rname) {
            return relation;
        }
    }
}

void DataLoader::show_sites() {
    for(auto site : this->sites) {
        std::cout << site << std::endl;
    }
    std::cout << std::endl;
}

void DataLoader::get_sites_from_etcd() {
    this->sites.clear();
    int site_num = this->read_site_num_from_etcd();
    std::unordered_map<std::string, std::string> site_info = read_from_etcd_by_prefix(this->key_map["sites"]);
    for(int i=0; i<site_num; ++i) {
        std::string prefix = this->key_map["sites"] + this->sep + std::to_string(i) + this->sep;
        std::string sname = site_info[prefix+"sname"];
        std::string ip = site_info[prefix+"ip"];
        std::string port = site_info[prefix+"port"];
        this->sites.push_back(Site(sname, ip, port));
    }
}

void DataLoader::add_site(std::string sname, std::string ip, std::string port) {
    // read_site_num_from_etcd
    int old_site_num = this->read_site_num_from_etcd();
    int new_site_num = old_site_num + 1;
    // build a key-value map
    std::map<std::string, std::string> m;
    m[this->key_map["site_num"]] = std::to_string(new_site_num);
    std::string prefix = this->key_map["sites"] + this->sep+std::to_string(old_site_num) + this->sep;
    m[prefix+"sname"] = sname;
    m[prefix+"ip"] = ip;
    m[prefix+"port"] = port;
    // write_map_to_etcd
    if(write_map_to_etcd(m) == 0) {
        this->sites.push_back(Site(sname, ip, port));
        std::cout << "Add site " << sname << " successfully.\n" << std::endl;
    } else {
        std::cout << "Add site " << sname << " failed.\n" << std::endl;
    }
}

int DataLoader::read_site_num_from_etcd() {
    std::string site_num = read_from_etcd_by_key(this->key_map["site_num"]);
    if(site_num == "") {
        int result = write_kv_to_etcd(this->key_map["site_num"], "0");
        return 0;
    }
    else {
        return std::stoi(site_num);
    }
}

void DataLoader::get_relations_from_etcd() {
    this->relations.clear();
    int relation_num = this->read_relation_num_from_etcd();
    std::unordered_map<std::string, std::string> relation_info = read_from_etcd_by_prefix(this->key_map["relations"]);
    for(int i=0; i<relation_num; ++i) {
        // get a instance of Relation
        std::string prefix = this->key_map["relations"] + this->sep + std::to_string(i) + this->sep;
        std::string rname = relation_info[prefix+"rname"];
        bool is_horizontal = (relation_info[prefix+"is_horizontal"] == "true") ? true : false;
        int num_of_recs = std::stoi(relation_info[prefix+"num_of_recs"]);
        Relation r(rname, is_horizontal, num_of_recs);
        // get attributes
        int num_of_attributes = std::stoi(relation_info[prefix+"num_of_attributes"]);
        for(int j=0; j<num_of_attributes; ++j) {
            prefix = this->key_map["relations"] + this->sep + std::to_string(i) + this->sep + "attributes" + this->sep + std::to_string(j) + this->sep;
            std::string aname = relation_info[prefix+"aname"];
            bool is_key = (relation_info[prefix+"is_key"] == "true") ? true : false;
            int type = std::stoi(relation_info[prefix+"type"]);
            if(relation_info[prefix+"value_type"] == "") {
                r.add_attribute(aname, is_key, type);
            } else {
                int value_type = std::stoi(relation_info[prefix+"value_type"]);
                if(value_type == 4) {
                    std::map<std::string, double> proportion;
                    int num_of_proportion = std::stoi(relation_info[prefix+"num_of_proportion"]);
                    for(int k=0; k<num_of_proportion; ++k) {
                        prefix = this->key_map["relations"] + this->sep + std::to_string(i) + this->sep + "attributes" + this->sep + std::to_string(j) + this->sep + "proportion" + this->sep + std::to_string(k) + this->sep;
                        std::string key = relation_info[prefix+"key"];
                        double value = std::stod(relation_info[prefix+"value"]);
                        proportion[key] = value;
                    }
                    r.add_attribute(aname, is_key, type, value_type, proportion);
                } else {
                    std::vector<double> value;
                    int num_of_value = std::stoi(relation_info[prefix+"num_of_values"]);
                    for(int k=0; k<num_of_value; ++k) {
                        double tem_value = std::stod(relation_info[this->key_map["relations"] + this->sep + std::to_string(i) + this->sep + "attributes" + this->sep + std::to_string(j) + this->sep + "value" + this->sep + std::to_string(k)]);
                        value.push_back(tem_value);
                    }
                    r.add_attribute(aname, is_key, type, value_type, value);
                }
            }
        }
        // get fragments
        prefix = this->key_map["relations"] + this->sep + std::to_string(i) + this->sep;
        int num_of_fragments = std::stoi(relation_info[prefix+"num_of_fragments"]);
        for(int j=0; j<num_of_fragments; ++j) {
            prefix = this->key_map["relations"] + this->sep + std::to_string(i) + this->sep + "fragments" + this->sep + std::to_string(j) + this->sep;
            std::string fname = relation_info[prefix+"fname"];
            std::string rname = relation_info[prefix+"rname"];
            std::string sname = relation_info[prefix+"sname"];
            bool is_horizontal = (relation_info[prefix+"is_horizontal"] == "true") ? true : false;
            int num_of_recs = std::stoi(relation_info[prefix+"num_of_recs"]);
            if(is_horizontal == true) {
                std::vector<Predicate> hf_condition;
                int num_of_hfc = std::stoi(relation_info[prefix+"num_of_hfc"]);
                for(int k=0; k<num_of_hfc; ++k) {
                    prefix = this->key_map["relations"] + this->sep + std::to_string(i) + this->sep + "fragments" + this->sep + std::to_string(j) + this->sep + "hf_condition" + this->sep + std::to_string(k) + this->sep;
                    int op_type = std::stoi(relation_info[prefix+"op_type"]);
                    std::string aname = relation_info[prefix+"aname"];
                    if(op_type == 6 || op_type == 9) {
                        std::string str = relation_info[prefix+"str"];
                        hf_condition.push_back(Predicate(op_type, aname, str));
                    } else {
                        double num = std::stod(relation_info[prefix+"num"]);
                        hf_condition.push_back(Predicate(op_type, aname, num));
                    }
                }
                r.add_fragment(rname, fname, sname, is_horizontal, hf_condition);
            } else {
                std::vector<std::string> vf_condition;
                int num_of_vfc = std::stoi(relation_info[prefix+"num_of_vfc"]);
                for(int k=0; k<num_of_vfc; ++k) {
                    std::string temp_vf_condition = relation_info[prefix+"vf_condition"+this->sep+std::to_string(k)];
                    vf_condition.push_back(temp_vf_condition);
                }
                r.add_fragment(rname, fname, sname, is_horizontal, vf_condition);
            }
        }
        // finally push the relation to vector
        this->relations.push_back(r);
    }
}

int DataLoader::read_relation_num_from_etcd() {
    std::string relation_num = read_from_etcd_by_key(this->key_map["relation_num"]);
    if(relation_num == "") {
        int result = write_kv_to_etcd(this->key_map["relation_num"], "0");
        return 0;
    }
    else {
        return std::stoi(relation_num);
    }
}

void DataLoader::add_relation(std::string rname, std::vector<Attribute> attributes) {
    // read_relation_num_from_etcd
    int old_relation_num = this->read_relation_num_from_etcd();
    int new_relation_num = old_relation_num + 1;
    // build a key-value map
    std::map<std::string, std::string> m;
    m[this->key_map["relation_num"]] = std::to_string(new_relation_num);
    std::string prefix = this->key_map["relations"] + this->sep + std::to_string(old_relation_num) + this->sep;
    m[prefix+"rname"] = rname;
    m[prefix+"is_horizontal"] = "true";
    m[prefix+"num_of_recs"] = "0";
    m[prefix+"num_of_attributes"] = std::to_string(attributes.size());
    m[prefix+"num_of_fragments"] = std::to_string(0);
    for(int i=0; i<attributes.size(); ++i) {
        prefix = this->key_map["relations"] + this->sep + std::to_string(old_relation_num) + this->sep + "attributes" + this->sep + std::to_string(i) + this->sep;
        m[prefix+"aname"] = attributes[i].aname;
        m[prefix+"is_key"] = (attributes[i].is_key == true) ? "true" : "false";
        m[prefix+"type"] = std::to_string(attributes[i].type);
    }
    // write_map_to_etcd
    if(write_map_to_etcd(m) == 0) {
        this->relations.push_back(Relation(rname, attributes));
        std::cout << "Add table " << rname << " successfully.\n" << std::endl;
    } else {
        std::cout << "Add table " << rname << " failed.\n" << std::endl;
    }
}

void DataLoader::add_unallocated_fragment(Fragment f) {
    std::unordered_map<std::string, Fragment>::const_iterator pos = this->unallocated_fragments.find(f.fname);
    if(pos == this->unallocated_fragments.end()) {
        if(!f.is_horizontal) {
            std::string prefix = this->get_prefix_by_rname(f.rname);
            write_kv_to_etcd(prefix+"is_horizontal", "false");
            for(auto& relation : this->relations) {
                if(relation.rname == f.rname) {
                    relation.is_horizontal = false;
                    break;
                }
            }
        }
        this->unallocated_fragments.insert(std::pair<std::string, Fragment>(f.fname, f));
        std::cout << "Add fragment " << f.fname << " successfully. It will be allocated after running `allocate [fragment_name] to [site_name]`.\n" << std::endl;
    } else {
        std::cout << "There exists a fragment named " << f.fname << ".\n" << std::endl;
    }
}

void DataLoader::add_fragment(Fragment f) {
    std::unordered_map<std::string, std::string> relation_info = read_from_etcd_by_prefix(this->key_map["relations"]);
    for(int i=0; i<this->relations.size(); ++i) {
        if(this->relations[i].rname == f.rname) {
            std::map<std::string, std::string> m;
            std::string prefix = this->get_prefix_by_rname(f.rname);
            int old_num_of_fragments = std::stoi(relation_info[prefix+"num_of_fragments"]);
            int new_num_of_fragments = old_num_of_fragments + 1;
            m[prefix+"is_horizontal"] = f.is_horizontal ? "true" : "false";
            m[prefix+"num_of_fragments"] = std::to_string(new_num_of_fragments);
            prefix = prefix + "fragments" + this->sep + std::to_string(old_num_of_fragments) + this->sep;
            m[prefix+"fname"] = f.fname;
            m[prefix+"rname"] = f.rname;
            m[prefix+"sname"] = f.sname;
            m[prefix+"is_horizontal"] = f.is_horizontal ? "true" : "false";
            m[prefix+"num_of_recs"] = std::to_string(0);
            if(f.is_horizontal) {
                int num_of_hfc = f.hf_condition.size();
                m[prefix+"num_of_hfc"] = std::to_string(num_of_hfc);
                for(int j=0; j<num_of_hfc; ++j) {
                    std::string tmp_prefix = prefix+"hf_condition"+this->sep+std::to_string(j)+this->sep;
                    int op_type = f.hf_condition[j].op_type;
                    m[tmp_prefix+"op_type"] = std::to_string(op_type);
                    m[tmp_prefix+"aname"] = f.hf_condition[j].aname;
                    if(op_type == 6 || op_type == 9) {
                        m[tmp_prefix+"str"] = f.hf_condition[j].str;
                    } else {
                        m[tmp_prefix+"num"] = double2string(f.hf_condition[j].num);
                    }
                }
            } else {
                int num_of_vfc = f.vf_condition.size();
                m[prefix+"num_of_vfc"] = std::to_string(num_of_vfc);
                for(int j=0; j<num_of_vfc; ++j) {
                    m[prefix+"vf_condition"+this->sep+std::to_string(j)] = f.vf_condition[j];
                }
            }
            if(write_map_to_etcd(m) == 0) {
                this->relations[i].add_fragment(f);
                this->unallocated_fragments.erase(f.fname);
                std::cout << "Allocate fragment " << f.fname << " to " << f.sname << " successfully.\n" << std::endl;
            } else {
                std::cout << "Allocate fragment " << f.fname << " to " << f.sname << " failed.\n" << std::endl;
            }
            return;
        } 
    }
}

void DataLoader::allocate(std::string fname, std::string sname) {
    bool exist_site = false;
    for(auto s : this->sites) {
        if(s.sname == sname) {
            exist_site = true;
            break;
        }
    }
    if(!exist_site) {
        std::cout << "There is no site named " << sname << ". Enter `show sites;` for the information of all sites;\n" << std::endl;
        return;
    }
    std::unordered_map<std::string, Fragment>::iterator pos = this->unallocated_fragments.find(fname);
    if(pos == this->unallocated_fragments.end()) {
        std::cout << "There is no unallocated fragment named " << fname << ".\n" << std::endl;
        return;
    } else {
        pos->second.set_sname(sname);
        this->add_fragment(pos->second);
    }
}

std::string DataLoader::get_prefix_by_rname(std::string rname) {
    int relation_num = this->read_relation_num_from_etcd();
    std::unordered_map<std::string, std::string> relation_info = read_from_etcd_by_prefix(this->key_map["relations"]);
    for(int i=0; i<relation_num; ++i) {
        // get a instance of Relation
        std::string prefix = this->key_map["relations"] + this->sep + std::to_string(i) + this->sep;
        if(rname == relation_info[prefix+"rname"])
            return prefix;
    }
    return "";
}

void DataLoader::show_unallocated_fragments() {
    for(std::unordered_map<std::string, Fragment>::iterator iter = this->unallocated_fragments.begin(); iter != this->unallocated_fragments.end(); iter++) {
        std::cout << iter->second << std::endl;
    }
    std::cout << std::endl;
}

std::unordered_map<std::string, std::string> DataLoader::get_site_to_insert(std::string rname, std::string values) {
    return this->get_relation_by_rname(rname).get_site_to_insert(values);
}

std::vector<Site> DataLoader::get_site_to_delete(std::string rname) {
    std::vector<Site> res;
    for(auto s : this->sites) {
        if(this->get_relation_by_rname(rname).in_site(s.sname)) {
            res.push_back(s);
        }
    }
    return res;
}

Site DataLoader::get_site_by_sname(const std::string& sname) {
    for(auto site : this->sites) {
        if(site.sname == sname) {
            return site;
        }
    }
}

std::map<std::string, std::string> DataLoader::get_sites_map() {
    std::map<std::string, std::string> site_map;
    for(auto s : this->sites) {
        site_map.insert(std::pair<std::string, std::string>(s.sname, s.ip));
    }
    return site_map;
}
