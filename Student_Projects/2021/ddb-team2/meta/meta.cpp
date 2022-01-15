/* meta.cpp

    Definitions of meta-data structures and related wrapping functions.

    Created by khaiwang

    2021/11/13
*/

#include "meta.h"

Site::Site(nlohmann::json site_json) {
    // site_address = base64_decode(site_json["value"]["site_address"]);
    // site_id = std::stoi(base64_decode(site_json["value"]["site_id"]));
    site_name = site_json["value"]["site_name"].get<std::string>();
    site_address = site_json["value"]["site_address"].get<std::string>();

    int frag_num = site_json["value"]["site_fragments"]["num"].get<int>();
    for(int i = 0; i < frag_num; i++) {
        std::string index =std::to_string(i);
        fragments.push_back(FragmentMeta(site_json["value"]["site_fragments"][index]));
    }
    
}
nlohmann::json Site::convert_to_json() {
    nlohmann::json j;
    // j["value"]["site_address"] = base64_encode(site_address);
    j["value"]["site_address"] = site_address;
    j["value"]["site_name"] = site_name;

    int i = 0;
    for(auto it = fragments.begin(); it != fragments.end(); it++){
        j["value"]["site_fragments"][std::to_string(i)] = it->convert_to_json();
        i++;
    }
    j["value"]["site_fragments"]["num"] = i;

    return j;
}

Field::Field(nlohmann::json field_json) {
    // field_name = base64_decode(field_json["value"]["field_name"]);
    // field_table_id = std::stoi(base64_decode(field_json["value"]["field_table_id"]));
    field_type = field_json["value"]["field_type"].get<std::string>();
    field_name = field_json["value"]["field_name"].get<std::string>();
}
nlohmann::json Field::convert_to_json() {
    nlohmann::json j;
    // j["value"]["field_name"] = base64_encode(field_name);
    j["value"]["field_name"] = field_name;
    j["value"]["field_type"] = field_type;
    return j;
}

TableMeta::TableMeta(nlohmann::json table_json) {
    // table_name = base64_decode(frag_json["value"]["table_name"]);
    // table_rows = std::stoi(base64_decode(frag_json["value"]["table_rows"]));
    // table_id = std::stoi(base64_decode(frag_json["value"]["table_id"]));
    // int frag_num = std::stoi(base64_decode(frag_json["value"]["table_fragments"]["num"]));
    table_name = table_json["value"]["table_name"].get<std::string>();
    table_rows = table_json["value"]["table_rows"].get<int>();
    table_id = table_json["value"]["table_id"].get<int>();
    create_table_sql = table_json["value"]["create_table_sql"].get<std::string>();
    int frag_num = table_json["value"]["table_fragments"]["num"].get<int>();
    for(int i = 0; i < frag_num; i++) {
        std::string index =std::to_string(i);
        table_fragments.push_back(FragmentMeta(table_json["value"]["table_fragments"][index]));
    }
    // int field_num = std::stoi(base64_decode(frag_json["value"]["table_fields"]["num"]));
    int field_num =table_json["value"]["table_fields"]["num"].get<int>();
    for(int i = 0; i < field_num; i++) {
        std::string index =std::to_string(i);
        table_fields.push_back(Field(table_json["value"]["table_fields"][index]));
    }
}

nlohmann::json TableMeta::convert_to_json() {
    nlohmann::json j;
    // j["value"]["table_name"] = base64_encode(table_name);
    // j["value"]["table_rows"] = base64_encode(std::to_string(table_rows));
    // j["value"]["table_id"] = base64_encode(std::to_string(table_id));
    j["value"]["table_name"] = table_name;
    j["value"]["table_rows"] = table_rows;
    j["value"]["table_id"] = table_id;
    j["value"]["create_table_sql"] = create_table_sql;
    int i = 0;
    for(auto it = table_fields.begin(); it != table_fields.end(); it++){
        j["value"]["table_fields"][std::to_string(i)] = it->convert_to_json();

        //j["value"]["table_fields"][std::to_string(i)]["value"].merge_patch(it->convert_to_json());

        i++;
    }
    j["value"]["table_fields"]["num"] = i;
    
    i = 0;
    for(auto it = table_fragments.begin(); it != table_fragments.end(); it++){
        j["value"]["table_fragments"][std::to_string(i)] = it->convert_to_json();
        i++;
    }
    j["value"]["table_fragments"]["num"] = i;
    return j;
}

FragmentMeta::FragmentMeta(nlohmann::json frag_json) {
    // frag_id = std::stoi(base64_decode(frag_json["value"]["frag_id"]));
    // frag_table_id = std::stoi(base64_decode(frag_json["value"]["frag_table_id"]));
    // frag_site_id = std::stoi(base64_decode(frag_json["value"]["frag_site_id"]));
    // is_horizontal = std::stoi(base64_decode(frag_json["value"]["is_horizontal"]));
    frag_name = frag_json["value"]["frag_name"].get<std::string>();
    frag_table_name = frag_json["value"]["frag_table_name"].get<std::string>();
    frag_site_name = frag_json["value"]["frag_site_name"].get<std::string>();
    frag_site_address = frag_json["value"]["frag_site_address"].get<std::string>();
    is_horizontal = frag_json["value"]["is_horizontal"].get<bool>();
    //std::cout << "test json bool " << base64_decode(frag_json["value"]["is_horizontal"]) << std::endl;
    Field field(frag_json["value"]["frag_on"]);
    // frag_on = field;
    frag_on.set_field_name(field.get_field_name());
    frag_on.set_field_type(field.get_field_type());
    // frag_range.first = base64_decode(frag_json["value"]["frag_range"]["start"]);
    // frag_range.second = base64_decode(frag_json["value"]["frag_range"]["end"]);
    frag_predicate = frag_json["value"]["frag_predicate"].get<std::string>();
    frag_create_table_sql = frag_json["value"]["frag_create_table_sql"].get<std::string>();
  
    int field_num = frag_json["value"]["frag_fields"]["num"].get<int>();
    for(int i = 0; i < field_num; i++) {
        std::string index =std::to_string(i);
        Field field(frag_json["value"]["frag_fields"][index]);
        frag_fields.push_back(field);
    }
}

nlohmann::json FragmentMeta::convert_to_json() {
    nlohmann::json j;
    j["value"]["frag_name"] = frag_name;
    j["value"]["frag_site_name"] = frag_site_name;
    j["value"]["frag_site_address"] = frag_site_address;
    j["value"]["frag_table_name"] = frag_table_name;
    
    j["value"]["frag_on"] = frag_on.convert_to_json();
    j["value"]["is_horizontal"] = is_horizontal;
    // j["value"]["frag_range"]["start"] = base64_encode(frag_range.first);
    // j["value"]["frag_range"]["end"] = base64_encode(frag_range.second);
    j["value"]["frag_predicate"] = frag_predicate;
    j["value"]["frag_create_table_sql"] = frag_create_table_sql;

    int i = 0;
    for(auto field : frag_fields){
        j["value"]["frag_fields"][std::to_string(i)] = field.convert_to_json();
        i++;
    }
    j["value"]["frag_fields"]["num"] = i;
    
    return j;
}

Node::Node(nlohmann::json node_json) {
    node_type = (Node_type)node_json["value"]["node_type"].get<int>();
    // select_predicate = base64_decode(node_json["value"]["select_predicate"].get<std::string>());
    // project_predicate = base64_decode(node_json["value"]["project_predicate"].get<std::string>());
    // join_predicate = base64_decode(node_json["value"]["join_predicate"].get<std::string>());
    is_union = node_json["value"]["is_union"].get<bool>();
    ret_code = node_json["value"]["ret_code"].get<int>();
    select_predicate = node_json["value"]["select_predicate"].get<std::string>();
    project_predicate = node_json["value"]["project_predicate"].get<std::string>();
    join_predicate = node_json["value"]["join_predicate"].get<std::string>();
    
    int children_num = node_json["value"]["children"]["num"].get<int>();
    for(int i  = 0; i < children_num; i++) {
        children[node_json["value"]["children"][std::to_string(i)]["key"].get<std::string>()] = 
            // base64_decode(node_json["value"]["children"][std::to_string(i)]["value"].get<std::string>());
            node_json["value"]["children"][std::to_string(i)]["value"].get<std::string>();
    }
}

nlohmann::json Node::convert_to_json() {
    nlohmann::json j;
    j["value"]["node_type"] = node_type;
    j["value"]["ret_code"] = ret_code;
    j["value"]["is_union"] = is_union;
    // j["value"]["select_predicate"] = base64_encode(select_predicate);
    // j["value"]["project_predicate"] = base64_encode(project_predicate);
    // j["value"]["join_predicate"] = base64_encode(join_predicate);
    j["value"]["select_predicate"] = select_predicate;
    j["value"]["project_predicate"] = project_predicate;
    j["value"]["join_predicate"] = join_predicate;
    int i = 0;
    for(auto iter = children.begin(); iter != children.end(); iter++) {
        // j["value"]["children"][std::to_string(i)][iter->first] = base64_encode(iter->second);
        j["value"]["children"][std::to_string(i)]["key"] = iter->first;
        j["value"]["children"][std::to_string(i)]["value"] = iter->second;
        i++;
    }
    j["value"]["children"]["num"] = i;
    return j;
}
