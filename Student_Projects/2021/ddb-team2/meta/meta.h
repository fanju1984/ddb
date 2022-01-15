/* meta.h

    Definitions of meta-data structures and related wrapping functions.

    Created by khaiwang

    2021/11/13
*/

// #include "network.h"
#include <vector>
#include <map>
#include <unordered_map>
#include "json.hpp"



class Field {
private:
    std::string field_name;
    std::string field_type;

public:
    Field(std::string type, std::string _field_name):field_type(type), field_name(_field_name){};
    Field(nlohmann::json field_json);
    Field():field_name(""), field_type("") {};
    //~Field();
    std::string get_field_type() {return field_type;}
    std::string get_field_name() {return field_name;}
    void set_field_type(std::string type){field_type = type;}
    void set_field_name(std::string name){field_name = name;}
    nlohmann::json convert_to_json(); 
    Field operator=(Field &field) {
        field_type = field.get_field_type();
        field_name = field.get_field_name();
    } 
};

class FragmentMeta {
private:
    std::string frag_name;
    std::string frag_table_name;
    std::string frag_site_name;
    std::string frag_site_address;
    std::vector<Field> frag_fields;
    bool is_horizontal;
    Field frag_on;
    std::string frag_predicate;
    std::string frag_create_table_sql;
    //~FragmentMeta();

public:
    FragmentMeta(std::string _frag_name, std::string _table_name, std::string _site_addres,
                    std::string _site_name, std::vector<Field> _fields, 
                    bool _horizontal, Field _frag_field, 
                    std::string _frag_predicate, std::string _create_sql):
                    frag_name(_frag_name), frag_table_name(_table_name), frag_site_address(_site_addres), 
                    frag_site_name(_site_name),
                    is_horizontal(_horizontal), frag_create_table_sql(_create_sql){
        for(auto field : _fields) {
            frag_fields.push_back(field);
        }
        frag_on.set_field_name(_frag_field.get_field_name());
        frag_on.set_field_type(_frag_field.get_field_type());
        frag_predicate = _frag_predicate;
        frag_create_table_sql = _create_sql;
    }
    FragmentMeta(nlohmann::json frag_json);
    std::string get_frag_name() {return frag_name;}
    std::string get_frag_table_name() {return frag_table_name;}
    std::string get_frag_site_name() {return frag_site_name;}
    std::string get_frag_site_address() {return frag_site_address;}
    bool frag_is_horizontal() {return is_horizontal;}
    Field get_frag_field() {return frag_on;}
    std::string get_frag_create_table_sql() {return frag_create_table_sql;}
    std::string get_frag_predicate() {return frag_predicate;}
    std::vector<Field> get_fields() {return frag_fields;}

    void set_frag_name(std::string name){frag_name = name;}
    void set_frag_table_name(std::string name){frag_table_name = name;}
    void set_frag_site_address(std::string address){frag_site_name = address;}
    void set_frag_site_name(std::string name){frag_site_name = name;}
    void set_is_horizontal(bool horizontal){is_horizontal = horizontal;}
    void set_frag_create_table_sql(std::string sql){frag_create_table_sql = sql;}

    void set_frag_field(Field field){frag_on = field;}
    void set_fields(std::vector<Field> fields) {
        for(auto field : fields) {
            frag_fields.push_back(field);
        }
    }
    void set_frag_predicate(std::string predicate) {
        frag_predicate = predicate;
    }

    nlohmann::json convert_to_json(); 
};

class Site {
private:
    std::string site_address;
    std::string site_name;
    std::vector<FragmentMeta> fragments;

public:
    Site(std::string address, std::string name):site_name(name), site_address(address){};
    Site(nlohmann::json site_json);
    std::string get_site_name() {return site_name;}
    std::string get_site_address() {return site_address;}

    void set_site_name(std::string name){site_name = name;}
    void set_site_address(std::string address){site_address = address;}
    void add_fragment(FragmentMeta frag) {
        fragments.push_back(frag);
    }
    std::vector<FragmentMeta> get_fragments() {return fragments;}
    void set_fragments(std::vector<FragmentMeta> _fragments) {
        for(auto fragment : _fragments) {
            fragments.push_back(fragment);
        }
    }

    nlohmann::json convert_to_json(); 
};


class TableMeta {
private:
    std::string table_name;
    std::vector<FragmentMeta> table_fragments;
    int table_rows;
    std::vector<Field> table_fields;
    int table_id;
    std::string create_table_sql;
    //~FragmentMeta();

public:
    TableMeta(std::string _name, int _rows, std::vector<Field> _fields, int _id, std::string sql):
        table_name(_name), table_rows(_rows), table_id(_id), create_table_sql(sql){
        for(auto field : _fields) {
            table_fields.push_back(field);
        }
    }
    TableMeta(nlohmann::json frag_json);
    int get_table_id() {return table_id;}
    int get_table_rows() {return table_rows;}
    std::string get_create_table_sql(){return create_table_sql;}
    std::string get_table_name() {return table_name;}
    std::vector<Field> get_table_fields() {return table_fields;}
    std::vector<FragmentMeta> get_fragments() {return table_fragments;}
    
    void set_create_table_sql(std::string sql) {create_table_sql = sql;}
    void set_table_id(int id){table_id = id;}
    void set_table_rows(int row){table_rows = row;}
    void set_table_name(int name){table_name = name;}

    void set_fields(std::vector<Field> fields) {
        for(auto field : fields) {
            table_fields.push_back(field);
        }
    }

    void add_fragment(FragmentMeta fragment) {table_fragments.push_back(fragment);}
    void set_fragments(std::vector<FragmentMeta> fragments) {
        for(auto fragment : fragments) {
            table_fragments.push_back(fragment);
        }
    }

    nlohmann::json convert_to_json(); 
};

enum Node_type {
    NODE_LEAF,
    NODE_NOT_LEAF
};

class Node {
private:
    int ret_code;
    std::string select_predicate;
    std::string join_predicate;
    std::string project_predicate;
    Node_type node_type;
    bool is_union;
    std::map<std::string, std::string> children;
public:
    Node(const int _ret_code, const std::string& _select_predicate, 
        const std::string& _join_predicate, const std::string& _project_predicate,
        const Node_type _type, const bool _is_union): 
        ret_code(_ret_code), select_predicate(_select_predicate),
        join_predicate(_join_predicate), project_predicate(_project_predicate), 
        node_type(_type), is_union(_is_union) {};
    Node(nlohmann::json node_json);
    int get_ret_code(){return ret_code;}
    void set_ret_code(const int _ret_code) {ret_code = _ret_code;}
    bool get_is_union(){return is_union;}
    void set_is_union(const bool _is_union) {is_union = _is_union;}
    std::string get_select_predicate() {return select_predicate;}
    void set_select_predicate(const std::string& _select_predicate) {select_predicate = _select_predicate;}
    std::string get_join_predicate() {return join_predicate;}
    void set_join_predicate(const std::string& _join_predicate) {join_predicate = _join_predicate;}
    std::string get_project_predicate() {return project_predicate;}
    void set_project_predicate(const std::string& _project_predicate) {project_predicate = _project_predicate;}
    Node_type get_node_type() {return node_type;}
    void set_node_type(const Node_type _node_type) {node_type = _node_type;}
    std::map<std::string, std::string> get_children() {return children;}
    void set_children(const std::map<std::string, std::string> _children) {children = _children;}
    void add_children(const std::string &key, const std::string &value){
        children[key] = value;
    }
    nlohmann::json convert_to_json(); 
};