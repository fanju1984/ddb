#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <set>
#include <boost/regex.hpp>

#include "sql-processor/SQLProcessor.h"
#include "data-loader/DataLoader.h"
#include "utils/utils.h"
#include "network-utils/network.h"
#include "sql-processor/SQLSelectProcessor.cpp"
#include "sql-processor/get_prefix.cpp"

const std::string ddbs_logo = "  _____  _____  ____   _____ \n"
                              " |  __ \\|  __ \\|  _ \\ / ____|\n"
                              " | |  | | |  | | |_) | (___  \n"
                              " | |  | | |  | |  _ < \\___ \\ \n"
                              " | |__| | |__| | |_) |____) |\n"
                              " |_____/|_____/|____/|_____/ \n";
const std::string ddbs_start = "Welcome to the DDBS monitor.\n"
                               "Commands end with `;`.\n"
                               "Type `help` or `h` for help.\n"
                               "For more information, visit: https://github.com/jaingmengmeng/DDBS\n";
const std::string ddbs_system = "ddbs";
const std::string ddbs_blank = "   -";
const std::string ddbs_bye = "Bye";
const std::string ddbs_command_help = "Command Usage:\t 1) `./main`\n"
                                      "\t\t 2) `./main <filename>`";
const std::string ddbs_help = "Usage:\t 1) `init`\n"
                              "\t 2) `show tables[;]`\n"
                              "\t 3) `load data[;]`\n"
                              "\t 4) `help` or `h`\n"
                              "\t 5) `select`\n"
                              "\t 6) `insert`\n"
                              "\t 7) `delete`\n"
                              "\t 8) `quit` or `q` or `exit`\n";
const std::string file_error = "Error opening file. Please check your filename.";
const std::string query_error = "The SQL string is invalid!";
std::string query = "";
std::string str = "";
int auto_increment_id = 0;
DataLoader data_loader = DataLoader();

enum INPUT_TYPE {
    QUIT,
    INIT,
    SHOW_TABLES,
    SHOW_FRAGMENTS,
    SHOW_SITES,
    HELP,
    DEFINE_SITE,
    CREATE_TABLE,
    FRAGMENT,
    ALLOCATE,
    SQL_STATE,
    LOAD_DATA,
    SET_DISTRIBUTION,
};

std::string execute_insert_sql(const std::string& sname, const std::string& rname, const std::string& values) {
    Site s = data_loader.get_site_by_sname(sname);
    std::string ip = s.ip;
    std::string insert_sql = "INSERT INTO " + s.sname+"_"+rname + " VALUES ("+ values +");";
    // std::cout << ip << " " << insert_sql << std::endl;
    return execute_non_query_sql(ip, insert_sql);
}

std::string execute_delete_sql(const std::string& sname, const std::string& rname, const std::vector<Predicate>& where) {
    Site s = data_loader.get_site_by_sname(sname);
    std::string ip = s.ip;
    std::string delete_sql = "DELETE FROM " + s.sname+"_"+rname;
    if(where.size() != 0) {
        delete_sql += " WHERE ";
        for(int i=0; i<where.size(); ++i) {
            if(i > 0)
                delete_sql += " AND ";
            delete_sql += where[i].to_string();
        }
    }
    // std::cout << ip << " " << delete_sql << std::endl;
    return execute_non_query_sql(ip, delete_sql);
}

std::string execute_create_sql(const std::string& sname, const std::string& rname) {
    Site s = data_loader.get_site_by_sname(sname);
    Relation r = data_loader.get_relation_by_rname(rname);
    std::string new_rname = sname + "_" + rname;
    std::string attr_meta = combine_vector_string(r.get_fragmented_attrs_meta(sname));
    std::string create_sql = "CREATE TABLE " + new_rname + " ("+ attr_meta +");";
    std::string ip = s.ip;
    // std::cout << ip << " " << create_sql << std::endl;
    return execute_non_query_sql(ip, create_sql);
}

std::string execute_drop_sql(const std::string& sname, const std::string& rname) {
    Site s = data_loader.get_site_by_sname(sname);
    Relation r = data_loader.get_relation_by_rname(rname);
    std::string new_rname = sname + "_" + rname;
    std::string drop_sql = "DROP TABLE IF EXISTS " + new_rname + ";";
    std::string ip = s.ip;
    // std::cout << ip << " " << drop_sql << std::endl;
    return execute_non_query_sql(ip, drop_sql);
}

void print_node(std::map<std::string, std::string>& tree_mp, std::string node, std::string site, int indent){
    if (indent != 0)
    {
        std::cout << "|"; 
    }
    for (int i = 0; i < indent; i++)
    {
        std::cout << "-";
    }

    std::string node_str = node + "(";
    node_str += site + ", ";
    std::string key = node + ".project";
    if (tree_mp.find(key) != tree_mp.end())
    {
        node_str += "project = " + tree_mp[key] + ", ";
    }
    key = node + ".select";
    if (tree_mp.find(key) != tree_mp.end())
    {
        node_str += "select = " + tree_mp[key] + ", ";
    }
    key = node + ".combine";
    if (tree_mp.find(key) != tree_mp.end())
    {
        node_str += "combine = " + tree_mp[key] + ", ";
    }
    node_str += ")";
    std::cout << node_str << "\n\n";
}

void print_query_tree(std::map<std::string, std::string>& tree_mp, std::map<string, string> node2site, std::string parent_node, int indent){

    print_node(tree_mp, parent_node, node2site[parent_node], indent);
  
    std::string children = tree_mp[parent_node + ".children"];
    std::string type = tree_mp[parent_node + ".type"];
    if (type == "L")
    {
        if (indent + 4 != 0)
        {
            std::cout << "|"; 
        }
        for (int i = 0; i < indent + 4; i++)
        {
            std::cout << "-";
        }
        std::cout << children << "\n\n";
    }
    else
    {
        std::string temp;
        int index1 = 0, index2 = 0;
        while (index2 != std::string::npos)
        {
            index2 = children.find("|", index1);
            // if (index2 != std::string::npos)
            // {
                temp = children.substr(index1, index2 - index1);
                int index3 = temp.find(":");
                std::string node = temp.substr(0, index3);
                print_query_tree(tree_mp, node2site, node, indent + 4);
                index1 = index2 + 1;
            // }
        }   
    }
}

void solve_single_query(std::string query) {
    std::cout << query << std::endl;
    SQLProcessor processor = SQLProcessor(query, data_loader.relations);
    if (processor.is_valid) {
        // select
        if(processor.sql_type == 1) {
            SelectStatement select_stat = processor.select;
            if(select_stat.from.size() == 1 && data_loader.get_relation_by_rname(select_stat.from[0]).num_of_recs == 0) {
                std::cout << "total: 0 rows" << std::endl;
                return;
            }
            std::map<std::string, std::string> select_tree;
            std::string prefix = get_prefix(auto_increment_id++);
            std::map<std::string, std::string> node2site;
            get_query_tree(select_tree, node2site, data_loader.relations, select_stat, prefix, data_loader.get_sites_map()); //get result in select_tree
            std::map<std::string, std::string>::reverse_iterator iter;
            // for(iter = select_tree.rbegin(); iter != select_tree.rend(); iter++){
            //     std::cout << iter->first << " " << iter->second << std::endl;
            // }
            
            write_map_to_etcd(select_tree);
            std::set<std::string> temp_tables;
            for(auto iter : select_tree){
                temp_tables.insert(iter.first.substr(0, iter.first.find('.')));
            }
            std::string root_temp_table = *(temp_tables.cbegin());
            node2site[root_temp_table] = "site" + std::to_string(get_site_no());
            // std::cout << "root: " << root_temp_table << std::endl;
            print_query_tree(select_tree, node2site, root_temp_table, 0);

            // [TODO]
            write_map_to_etcd(select_tree);
            std::vector<std::string> rows = request_table(root_temp_table);
            // for(const std::string& row : rows){
            //     std::cout << row << std::endl;
            // }
            std::cout << "total: " << rows.size() << " rows" << std::endl;
            std::vector<std::string> v(temp_tables.cbegin(), temp_tables.cend());
            try
            {
                std::map<std::string, std::string> statistics = get_request_statistics(v);
                // latency and cc
                std::string lm = statistics[root_temp_table];
                // std::cout << "root temp table lm: " << lm << std::endl;
                long latency = std::stol(lm.substr(0, lm.find(',')));
                long cc = 0;
                for(auto iter : statistics){
                    // std::cout << iter.first << ": " << iter.second << std::endl;
                    cc += std::stol(iter.second.substr(iter.second.find(',') + 1));
                }
                std::cout << "latency(MS): " << latency / 1000.0 << std::endl;
                std::cout << "communication cost(KB): " << cc / 1000.0 << std::endl;
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }

            // delete query tree in etcd
            delete_from_etcd_by_prefix(root_temp_table);
        }
        // insert
        else if(processor.sql_type == 2) {
            InsertStatement insert_stat = processor.insert;
            // get sites with values to insert.
            std::unordered_map<std::string, std::string> insert_map = data_loader.get_site_to_insert(insert_stat.rname, insert_stat.values);
            std::unordered_map<std::string, std::string>::const_iterator iter;
            for(iter = insert_map.begin(); iter != insert_map.end(); iter++) {
                std::cout << "(" << iter->second << ")" << " is inserted into " << iter->first << std::endl;
                std::string result = execute_insert_sql(iter->first, insert_stat.rname, iter->second);
                std::cout << result << std::endl;
            }
            // update num_of_recs
            std::string prefix = data_loader.get_prefix_by_rname(insert_stat.rname);
            std::string key = prefix + "num_of_recs";
            int num_of_recs = std::stoi(read_from_etcd_by_key(key));
            write_kv_to_etcd(key, std::to_string(num_of_recs+1));
            data_loader.get_relations_from_etcd();
        }
        // delete
        else if(processor.sql_type == 3) {
            DeleteStatement delete_stat = processor.delete_s;
            std::vector<Site> v_site = data_loader.get_site_to_delete(delete_stat.rname);
            Relation r = data_loader.get_relation_by_rname(delete_stat.rname);
            if(delete_stat.where.size() > 0) {
                int count = 0;
                if(r.is_horizontal) {
                    for(auto s : v_site) {
                        // excute select sql to get rows to be deleted.
                        std::string select_sql = "SELECT * FROM " + s.sname+"_"+delete_stat.rname;
                        select_sql += " WHERE ";
                        for(int i=0; i<delete_stat.where.size(); ++i) {
                            if(i>0) {
                                select_sql += " AND ";
                            }
                            select_sql += delete_stat.where[i].to_string();
                        }
                        // std::cout << select_sql << std::endl;
                        std::vector<std::string> res = execute_query_sql(s.ip, select_sql);
                        if(res.size() > 1) {
                            for(int i=1; i<res.size(); ++i) {
                                std::cout << "(" << res[i] << ") is deleted from " << s.sname << std::endl;
                                count++;
                            }
                        }
                        // excute delete sql
                        std::string result = execute_delete_sql(s.sname, delete_stat.rname, delete_stat.where);
                        if(res.size() > 1) std::cout << result << std::endl;
                    }
                }
                // (id, name)  (id, rank) get the id of rows which to delete.
                else {
                    std::vector<std::vector<std::string>> v_res;
                    Relation r = data_loader.get_relation_by_rname(delete_stat.rname);
                    std::string key = r.get_key();
                    int key_type = r.get_key_type();
                    for(auto p : delete_stat.where) {
                        for(auto f : r.frags) {
                            for(auto a : f.vf_condition) {
                                if(a == p.aname) {
                                    Site s = data_loader.get_site_by_sname(f.sname);
                                    std::string select_sql = "SELECT * FROM " + s.sname+"_"+delete_stat.rname;
                                    select_sql += " WHERE ";
                                    select_sql += p.to_string();
                                    std::vector<std::string> res = execute_query_sql(s.ip, select_sql);
                                    // get the key of rows that to be deleted
                                    std::vector<std::string> res_key;
                                    std::string attr_meta = res[0];
                                    std::vector<std::string> v_attr;
                                    split_string(attr_meta, v_attr, ",");
                                    int key_index = 0;
                                    for(int j=0; j<v_attr.size(); ++j) {
                                        if(v_attr[j].substr(0, key.size()) == key) {
                                            key_index = j;
                                            break;
                                        }
                                    }
                                    if(res.size() > 1) {
                                        for(int j=1; j<res.size(); ++j) {
                                            std::vector<std::string> v_value;
                                            split_string(res[j], v_value, ",");
                                            res_key.push_back(v_value[key_index]);
                                        }
                                    }
                                    v_res.push_back(res_key);
                                }
                            }
                        }
                    }
                    // get intersection
                    std::vector<std::string> v_key = get_intersection(v_res);
                    count = v_key.size();
                    for(auto k : v_key) {
                        if(key_type == 2) {
                            k = "'" + k + "'";
                        }
                        for(auto s : v_site) {
                            std::string select_sql = "SELECT * FROM " + s.sname+"_"+delete_stat.rname;
                            select_sql += " WHERE ";
                            select_sql += (key + "=" + k + ";");
                            std::vector<std::string> res = execute_query_sql(s.ip, select_sql);
                            std::cout << "(" << res[1] << ") is deleted from " << s.sname << std::endl;
                            // excute delete sql
                            std::string result = execute_delete_sql(s.sname, delete_stat.rname, std::vector<Predicate>{Predicate(6, key, k)});
                            std::cout << result << std::endl;
                        }
                    }
                }
                // update num_of_recs
                std::string prefix = data_loader.get_prefix_by_rname(delete_stat.rname);
                std::string key = prefix + "num_of_recs";
                int num_of_recs = std::stoi(read_from_etcd_by_key(key));
                write_kv_to_etcd(key, std::to_string(num_of_recs-count));
                data_loader.get_relations_from_etcd();
            }
            // delete all rows
            else {
                // get sites with conditions to delete.
                for(auto s : v_site) {
                    std::string delete_sql = "DELETE FROM " + s.sname + "_" + delete_stat.rname;
                    std::cout << delete_sql << std::endl;
                    std::string result = execute_non_query_sql(s.ip, delete_sql);
                    std::cout << result << std::endl;
                }
                // update num_of_recs
                std::string prefix = data_loader.get_prefix_by_rname(delete_stat.rname);
                std::string key = prefix + "num_of_recs";
                int num_of_recs = std::stoi(read_from_etcd_by_key(key));
                write_kv_to_etcd(key, std::to_string(0));
                data_loader.get_relations_from_etcd();
            }
        }
    } else {

    }
}

INPUT_TYPE input_classifier(std::string input) {
    boost::regex re_quit("^((q(uit)?)|(e(xit)?))\\s*;?$", boost::regex::icase);
    boost::regex re_init("^init\\s*;?$", boost::regex::icase);
    boost::regex re_show_tables("^show\\s+tables\\s*;?$", boost::regex::icase);
    boost::regex re_show_fragments("^show\\s+fragments\\s*;?$", boost::regex::icase);
    boost::regex re_show_sites("^show\\s+sites\\s*;?$", boost::regex::icase);
    boost::regex re_help("^h(elp)?\\s*;?$", boost::regex::icase);
    boost::regex re_define_site("^define\\s+site\\s+[A-Za-z0-9]+\\s+[0-9.]+:[0-9]+(\\s*,\\s*[A-Za-z0-9]+\\s+[0-9.]+:[0-9]+)*\\s*;?$", boost::regex::icase);
    boost::regex re_create_table("^create\\s+table\\s+[A-Za-z0-9]+\\s*\\(\\s*[A-Za-z_]+\\s+(int|char\\s*\\(\\s*[0-9]+\\s*\\))(\\s+key)?(\\s*,\\s*[A-Za-z_]+\\s+(int|char\\s*\\(\\s*[0-9]+\\s*\\))(\\s+key)?\\s*)*\\s*\\)\\s*;?$", boost::regex::icase);
    boost::regex re_fragment("^fragment\\s+[A-Za-z0-9]+\\s+(horizontally|vertically)\\s+into\\s+[^;]+\\s*;?$", boost::regex::icase);
    boost::regex re_allocate("^allocate\\s+[A-Za-z0-9\\.]+\\s+to\\s+[A-Za-z0-9]+\\s*;?$", boost::regex::icase);
    boost::regex re_load("^load\\s+data\\s+local\\s+infile\\s+(('[A-Za-z0-9_/\\.]+')|(\"[A-Za-z0-9_/\\.]+\"))\\s+into\\s+table\\s+[A-Za-z0-9]+\\s*;?$", boost::regex::icase);
    boost::regex re_set_distribution("^set\\s+distribution\\s*;?$", boost::regex::icase);
    if(boost::regex_match(input, re_quit)) {
        return QUIT;
    } else if(boost::regex_match(input, re_init)) {
        return INIT;
    } else if(boost::regex_match(input, re_show_tables)) {
        return SHOW_TABLES;
    } else if(boost::regex_match(input, re_show_fragments)) {
        return SHOW_FRAGMENTS;
    } else if(boost::regex_match(input, re_show_sites)) {
        return SHOW_SITES;
    } else if(boost::regex_match(input, re_help)) {
        return HELP;
    } else if(boost::regex_match(input, re_define_site)) {
        return DEFINE_SITE;
    } else if(boost::regex_match(input, re_create_table)) {
        return CREATE_TABLE;
    } else if(boost::regex_match(input, re_fragment)) {
        return FRAGMENT;
    } else if(boost::regex_match(input, re_allocate)) {
        return ALLOCATE;
    } else if(boost::regex_match(input, re_load)) {
        return LOAD_DATA;
    } else if(boost::regex_match(input, re_set_distribution)) {
        return SET_DISTRIBUTION;
    } else {
        return SQL_STATE;
    }
}

int solve(bool show_query=false) {
    if(query != "")
        query += " ";
    query += str;
    query = trim(query);
    if(show_query) std::cout << query << std::endl;
    INPUT_TYPE input_type = input_classifier(query);
    if(input_type == DEFINE_SITE) {
        query = lower_string(query);
        // delete the `define site` string
        boost::regex tmp_define_site("(define\\s+site\\s+)([^;]+)(;?)");
        query = boost::regex_replace(query, tmp_define_site, "$2");
        std::vector<std::string> v_sites;
        split_string(query, v_sites, ",");
        for(auto site : v_sites) {
            // get sname, ip, port
            site = trim(site);
            std::string sname = lower_string(trim(site.substr(0, site.find_first_of(" ")-0)));
            std::string ip =  trim(site.substr(site.find_first_of(" ")+1, site.find_first_of(":")-site.find_first_of(" ")-1));
            std::string port =  trim(site.substr(site.find(":")+1));
            // add site
            data_loader.add_site(sname, ip, port);
        }
        // initial variables
        query = "";
        std::cout << ddbs_system << "> ";
    } else if(input_type == CREATE_TABLE) {
        query = lower_string(query);
        // delete the `create table` string
        boost::regex tmp_create_table("(create\\s+table\\s+)([^;]+)(;?)");
        query = trim(boost::regex_replace(query, tmp_create_table, "$2"));
        // get rname
        std::string rname = trim(query.substr(0, query.find_first_of("(")));
        // get attributes
        std::vector<Attribute> attributes;
        std::vector<std::string> v_attributes;
        std::string attributes_str = query.substr(query.find_first_of("(")+1, query.find_last_of(")")-query.find_first_of("(")-1);
        // std::cout << attributes_str << std::endl;
        split_string(attributes_str, v_attributes, ",");
        for(auto attribute : v_attributes) {
            attribute = trim(attribute);
            // std::cout << attribute << std::endl;
            std::vector<std::string> v_str;
            split_string(attribute, v_str, " ");
            for(auto& str : v_str) {
                str = trim(str);
                // std::cout << "#" << str << "#" << std::endl;
            }
            bool is_key = v_str.size() == 3 && v_str[2] == "key";
            int type = (v_str[1] == "int") ? 1 : 2;
            // std::cout << v_str[0] << " " << is_key << " " << type << "#" << std::endl;
            attributes.push_back(Attribute(v_str[0], is_key, type));
        }
        // add relation
        data_loader.add_relation(rname, attributes);
        // initial variables
        query = "";
        std::cout << ddbs_system << "> ";
    } else if(input_type == FRAGMENT) {
        // get is_horizontal & fragment conditions
        boost::regex tmp_fragment("(fragment\\s+)([A-Za-z0-9]+\\s+)(horizontally|vertically)(\\s+into\\s+)([^;]+)(\\s*;?)");
        std::string rname = lower_string(trim(boost::regex_replace(query, tmp_fragment, "$2")));
        bool is_horizontal = (trim(boost::regex_replace(query, tmp_fragment, "$3")) == "horizontally") ? true : false;
        std::string conditions = trim(boost::regex_replace(query, tmp_fragment, "$5"));
        // add fragments
        if(is_horizontal) {
            std::vector<std::string> v_condition;
            split_string(conditions, v_condition, ",");
            for(int i=0; i<v_condition.size(); ++i) {
                std::string fname = rname+"."+std::to_string(i+1);
                std::vector<Predicate> hf_condition;
                std::vector<std::string> v_predicate;
                split_string(v_condition[i], v_predicate, "and");
                // build a fragment instance and add fragment
                for(int j=0; j<v_predicate.size(); ++j) {
                    boost::regex tmp_predicate("([A-Za-z_]+\\s*)(>|>=|<|<=|=|<>)(\\s*[\"'A-Za-z0-9]+)");
                    std::string aname = trim(boost::regex_replace(v_predicate[j], tmp_predicate, "$1"));
                    std::string op = trim(boost::regex_replace(v_predicate[j], tmp_predicate, "$2"));
                    std::string value = trim(boost::regex_replace(v_predicate[j], tmp_predicate, "$3"));
                    // std::cout << aname << " " << op << " " << value << std::endl;
                    int op_type;
                    if(value[0] == '\'' || value[0] == '"') {
                        if(op == "=") op_type = 6;
                        else if(op == "<>") op_type = 9;
                        hf_condition.push_back(Predicate(op_type, aname, value.substr(1, value.size()-2)));
                    } else {
                        if(op == ">=") op_type = 1;
                        else if(op == "<=") op_type = 2;
                        else if(op == ">") op_type = 3;
                        else if(op == "<") op_type = 4;
                        else if(op == "=") op_type = 5;
                        else if(op == "<>") op_type = 8;
                        hf_condition.push_back(Predicate(op_type, aname, std::stod(value)));
                    }
                }
                // [TODO] to vertify the fragment statement is valid --- the fragment is complete & nonexistent.
                data_loader.add_unallocated_fragment(Fragment(rname, fname, is_horizontal, hf_condition));
            }
        } else {
            std::vector<std::string> v_condition;
            while(conditions.find_first_of("(") != std::string::npos) {
                std::string tem_condition = conditions.substr(conditions.find_first_of("(")+1, conditions.find_first_of(")")-conditions.find_first_of("(")-1);
                v_condition.push_back(tem_condition);
                conditions = conditions.substr(conditions.find_first_of(")")+1);
            }
            for(int i=0; i<v_condition.size(); ++i) {
                std::string fname = rname+"."+std::to_string(i+1);
                std::vector<std::string> v_aname;
                v_condition[i] = trim(v_condition[i].substr(v_condition[i].find_first_of("(")+1, v_condition[i].find_first_of(")")-v_condition[i].find_first_of("(")-1));
                split_string(v_condition[i], v_aname, ",");
                std::vector<std::string> vf_condition;
                for(int j=0; j<v_aname.size(); ++j) {
                    std::string aname = trim(v_aname[j]);
                    vf_condition.push_back(aname);
                }
                data_loader.add_unallocated_fragment(Fragment(rname, fname, is_horizontal, vf_condition));
            }
        }
        // initial variables
        query = "";
        std::cout << ddbs_system << "> ";
    } else if(input_type == ALLOCATE) {
        query = lower_string(query);
        // get fname & sname
        boost::regex tmp_allocate("(allocate\\s+)([A-Za-z0-9\\.]+)(\\s+to\\s+)([A-Za-z0-9]+)(\\s*;?)");
        std::string fname = trim(boost::regex_replace(query, tmp_allocate, "$2"));
        std::string sname = trim(boost::regex_replace(query, tmp_allocate, "$4"));
        // std::cout << fname << " " << sname << "#" << std::endl;
        // allocate the fragment to the site
        data_loader.allocate(fname, sname);
        execute_create_sql(sname, fname.substr(0, fname.find_first_of(".")));
        // initial variables
        query = "";
        std::cout << ddbs_system << "> ";
    } else if(input_type == LOAD_DATA) {
        query = lower_string(query);
        // get file_path & rname
        boost::regex tmp_load("(load\\s+data\\s+local\\s+infile\\s+')([A-Za-z0-9_/\\.]+)('\\s+into\\s+table\\s+)([A-Za-z0-9]+)(\\s*;?)");
        std::string file_path = trim(boost::regex_replace(query, tmp_load, "$2"));
        std::string rname = trim(boost::regex_replace(query, tmp_load, "$4"));
        Relation r = data_loader.get_relation_by_rname(rname);
        // read data from file
        std::vector<std::vector<std::string>> new_data = data_loader.get_data_from_tsv(file_path);
        // fragment data
        std::map<std::string, std::vector<std::vector<std::string>>> fragment_data_map = data_loader.fragment_data(rname, new_data);
        std::map<std::string, std::vector<std::vector<std::string>>>::iterator iter;
        for(iter = fragment_data_map.begin(); iter != fragment_data_map.end(); iter++) {
            std::string sname = iter->first;
            Site s = data_loader.get_site_by_sname(sname);
            std::string url = s.get_url();
            std::string attr_meta = combine_vector_string(r.get_fragmented_attrs_meta(sname));
            std::vector<std::vector<std::string>> new_data = iter->second;
            std::vector<std::string> insert_values = data_loader.get_insert_values(rname, new_data);
            int count = insert_values.size();
            int res = load_table(url, sname+std::string("_")+rname, attr_meta, insert_values);
            // update num_of_recs
            std::string prefix = data_loader.get_prefix_by_rname(rname);
            std::string key = prefix + "num_of_recs";
            int num_of_recs = std::stoi(read_from_etcd_by_key(key));
            write_kv_to_etcd(key, std::to_string(num_of_recs+count));
            data_loader.get_relations_from_etcd();
        }
        // initial variables
        query = "";
        std::cout << ddbs_system << "> ";
    } else if(input_type == SET_DISTRIBUTION) {
        std::unordered_map<std::string, std::unordered_map<std::string, std::string>> distribution_map;
        std::unordered_map<std::string, std::string> book_map;
        std::unordered_map<std::string, std::string> customer_map;
        std::unordered_map<std::string, std::string> orders_map;
        std::unordered_map<std::string, std::string> publisher_map;
        // book.id
        book_map["attributes/0/value_type"] = "1";
        book_map["attributes/0/num_of_values"] = "2";
        book_map["attributes/0/value/0"] = "200001";
        book_map["attributes/0/value/1"] = "250000";
        // book.publisher_id
        book_map["attributes/3/value_type"] = "1";
        book_map["attributes/3/num_of_values"] = "2";
        book_map["attributes/3/value/0"] = "100001";
        book_map["attributes/3/value/1"] = "105000";
        // book.copies
        book_map["attributes/4/value_type"] = "2";
        book_map["attributes/4/num_of_values"] = "2";
        book_map["attributes/4/value/0"] = "0";
        book_map["attributes/4/value/1"] = "10000";
        // customer.id
        customer_map["attributes/0/value_type"] = "1";
        customer_map["attributes/0/num_of_values"] = "2";
        customer_map["attributes/0/value/0"] = "300001";
        customer_map["attributes/0/value/1"] = "315000";
        // customer.rank
        customer_map["attributes/2/value_type"] = "4";
        customer_map["attributes/2/num_of_proportion"] = "3";
        customer_map["attributes/2/proportion/0/key"] = "1";
        customer_map["attributes/2/proportion/0/value"] = "0.4";
        customer_map["attributes/2/proportion/1/key"] = "2";
        customer_map["attributes/2/proportion/1/value"] = "0.3";
        customer_map["attributes/2/proportion/2/key"] = "3";
        customer_map["attributes/2/proportion/2/value"] = "0.3";
        // orders.customer_id
        orders_map["attributes/0/value_type"] = "1";
        orders_map["attributes/0/num_of_values"] = "2";
        orders_map["attributes/0/value/0"] = "300001";
        orders_map["attributes/0/value/1"] = "315000";
        // orders.book_id
        orders_map["attributes/1/value_type"] = "1";
        orders_map["attributes/1/num_of_values"] = "2";
        orders_map["attributes/1/value/0"] = "200001";
        orders_map["attributes/1/value/1"] = "250000";
        // orders.quantity
        orders_map["attributes/2/value_type"] = "3";
        orders_map["attributes/2/num_of_values"] = "2";
        orders_map["attributes/2/value/0"] = "3";
        orders_map["attributes/2/value/1"] = "2";
        // publisher.id
        publisher_map["attributes/0/value_type"] = "1";
        publisher_map["attributes/0/num_of_values"] = "2";
        publisher_map["attributes/0/value/0"] = "100001";
        publisher_map["attributes/0/value/1"] = "105000";
        // publisher.nation
        publisher_map["attributes/2/value_type"] = "4";
        publisher_map["attributes/2/num_of_proportion"] = "2";
        publisher_map["attributes/2/proportion/0/key"] = "PRC";
        publisher_map["attributes/2/proportion/0/value"] = "0.5";
        publisher_map["attributes/2/proportion/1/key"] = "USA";
        publisher_map["attributes/2/proportion/1/value"] = "0.5";

        distribution_map["book"] = book_map;
        distribution_map["customer"] = customer_map;
        distribution_map["orders"] = orders_map;
        distribution_map["publisher"] = publisher_map;
        std::map<std::string, std::string> kv_map;
        for(auto iter=distribution_map.begin(); iter!=distribution_map.end(); iter++) {
            std::string rname = iter->first;
            std::string prefix = data_loader.get_prefix_by_rname(rname);
            for(auto iter_2=iter->second.begin(); iter_2!=iter->second.end(); iter_2++) {
                kv_map[prefix+iter_2->first] = iter_2->second;
            }
        }
        write_map_to_etcd(kv_map);
        data_loader.get_relations_from_etcd();
        // initial variables
        query = "";
        std::cout << ddbs_system << "> ";
    } else if(input_type == INIT) {
        for(auto r : data_loader.relations) {
            for(auto f : r.frags) {
                execute_drop_sql(f.sname, f.rname);
            }
        }
        data_loader.init();
        // initial variables
        query = "";
        std::cout << ddbs_system << "> ";
    } else if(input_type == SHOW_TABLES) {
        data_loader.show_tables();
        // initial variables
        query = "";
        std::cout << ddbs_system << "> ";
    } else if(input_type == SHOW_FRAGMENTS) {
        data_loader.show_unallocated_fragments();
        // initial variables
        query = "";
        std::cout << ddbs_system << "> ";
    } else if(input_type == SHOW_SITES) {
        data_loader.show_sites();
        // initial variables
        query = "";
        std::cout << ddbs_system << "> ";
    } else if(input_type == HELP) {
        std::cout << ddbs_help << std::endl;
        // initial variables
        query = "";
        std::cout << ddbs_system << "> ";
    } else if(input_type == QUIT) {
        std::cout << ddbs_bye << std::endl;
        return 0;
    } else {
        // insert; delete; select;
        if(query[query.size()-1] == ';') {
            // process the query statements
            solve_single_query(query);
            // initial variables
            query = "";
            std::cout << ddbs_system << "> ";
        } else {
            std::cout << ddbs_blank << "> ";
        }
    }
    return 1;
}

int main(int argc, char *argv[]) {
    // read sql query from std input
    if(argc == 1) {
        std::cout << ddbs_logo << std::endl << ddbs_start << std::endl;
        std::cout << ddbs_system << "> ";
        while(std::getline(std::cin, str)) {
            if(solve() == 0) {
                return 0;
            }
        }
    } else if(argc == 2) {
        std::cout << ddbs_logo << std::endl << ddbs_start << std::endl;
        std::cout << ddbs_system << "> ";
        std::ifstream fin(argv[1], std::ios_base::in);
        if (fin.is_open()) {
            while(std::getline(fin, str)) {
                if(solve(true) == 0) {
                    return 0;
                }
            }
        } else {
            std::cout << file_error << std::endl;
            exit(1);
        }
    } else {
        std::cout << ddbs_command_help << std::endl;
    }
    return 0;
}
