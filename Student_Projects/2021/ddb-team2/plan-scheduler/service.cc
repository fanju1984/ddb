#include <gflags/gflags.h>
#include "brpc/channel.h"
#include "butil/logging.h"

#include <iostream>
#include <string>
#include <vector>
#include "ddb.pb.h"
#include <fstream>
#include <sstream>
#include "meta.h"
#include "network.h"
#include <regex>
#include <map>
#include <unordered_map>

// DEFINE_string(temp_table, "1-temp-table", "Temp table name.");
DEFINE_string(protocol_service, "baidu_std", "Protocol type. Defined in src/brpc/options.proto");
DEFINE_string(connection_type_service, "", "Connection type. Available values: single, pooled, short");
DEFINE_string(server_service, "127.0.0.1", "IP Address of server");
DEFINE_string(load_balancer_service, "", "The algorithm for load balancing");
DEFINE_int32(timeout_ms_service, 300000, "RPC timeout in milliseconds");
DEFINE_int32(max_retry_service, 3, "Max retries(not including the first RPC)");
DEFINE_int32(interval_ms_service, 1000, "Milliseconds between consecutive requests");
DEFINE_int32(port_server, 8000, "the port of server");
DEFINE_string(sql, "select book.title from book where copies>5000;", "sql query");
DEFINE_bool(init_all, false, "init the environment");
DEFINE_bool(verbose, false, "print sql query result");
DEFINE_bool(load, false, "if load data");
DEFINE_bool(test_delete, false, "if insert data");
DEFINE_bool(test_insert, false, "if delete data");
DEFINE_bool(test_sql, true, "if test sql");



const std::string SEPERATE_LINE = "===================================";
const std::string SEPERATE_SIGN = "* ";


void show_plan_tree(const std::string &root){
    std::cout << SEPERATE_LINE << std::endl;
    std::string root_node = read_kv_from_etcd(root);
    nlohmann::json root_json = nlohmann::json::parse(root_node);
    std::cout << SEPERATE_SIGN << root << std::endl;
    std::cout << root_json.dump() << std::endl;
    if(root_json["value"]["node_type"].get<int>()==0){
        return;
    }
    int children_num = root_json["value"]["children"]["num"].get<int>();
    std::vector<std::string> children;
    
    for(int i  = 0; i < children_num; i++) {
        show_plan_tree(root_json["value"]["children"][std::to_string(i)]["key"].get<std::string>());
    }
    
}

// std::regex re_quit("\\s*((q(uit)?)|(e(xit)?))\\s*;?$", std::regex::icase);
// std::regex re_init("\\s*init\\s*;?$", std::regex::icase);
// std::regex re_show_tables("^show\\s+tables\\s*;?$", std::regex::icase);
// std::regex re_show_fragments("^show\\s+fragments\\s*;?$", std::regex::icase);
// std::regex re_show_sites("^show\\s+sites\\s*;?$", std::regex::icase);
// std::regex re_help("^h(elp)?\\s*;?$", std::regex::icase);
// std::regex re_define_site("^define\\s+site\\s+[A-Za-z0-9]+\\s+[0-9.]+:[0-9]+(\\s*,\\s*[A-Za-z0-9]+\\s+[0-9.]+:[0-9]+)*\\s*;?$", std::regex::icase);
// std::regex re_create_table("^create\\s+table\\s+[A-Za-z0-9]+\\s*\\(\\s*[A-Za-z_]+\\s+(int|char\\s*\\(\\s*[0-9]+\\s*\\))(\\s+key)?(\\s*,\\s*[A-Za-z_]+\\s+(int|char\\s*\\(\\s*[0-9]+\\s*\\))(\\s+key)?\\s*)*\\s*\\)\\s*;?$", std::regex::icase);
// std::regex re_fragment("^fragment\\s+[A-Za-z0-9]+\\s+(horizontally|vertically)\\s+into\\s+[^;]+\\s*;?$", std::regex::icase);
// std::regex re_allocate("^allocate\\s+[A-Za-z0-9\\.]+\\s+to\\s+[A-Za-z0-9]+\\s*;?$", std::regex::icase);
// std::regex re_load("^load\\s+data\\s+local\\s+infile\\s+(('[A-Za-z0-9_/\\.]+')|(\"[A-Za-z0-9_/\\.]+\"))\\s+into\\s+table\\s+[A-Za-z0-9]+\\s*;?$", std::regex::icase);
// std::regex re_set_distribution("^set\\s+distribution\\s*;?$", std::regex::icase);

// std::regex re_select("\\s*select\\s+(.*)?\\s+from\\s+(.*)?\\s*;\\s*", std::regex::icase);
// std::regex re_select_where("\\s*select\\s+(.*)?\\s+from\\s+(.*)?\\s+where\\s+(.*)?\\s*;\\s*", std::regex::icase);
std::regex re_define_site("\\s*define\\s+site\\s+(.+)?\\s+address\\s+(.+)?\\s*;\\s*", std::regex::icase);
std::regex re_create_db("\\s*create\\s+database\\s+(.+)?\\s*;\\s*", std::regex::icase);
std::regex re_fragment_h("\\s*create\\s+horizontal\\s+fragment\\s+(.+)?\\s+of\\s+(.+)?\\s+on\\s+(.+)?\\s+frag\\s+on\\s+(.+)?where(.+)?\\s*;\\s*", std::regex::icase);
std::regex re_fragment_v("\\s*create\\s+vertical\\s+fragment\\s+(.+)?\\s+of\\s+(.+)?\\s+on\\s+(.+)?\\s+frag\\s+on\\s+(.+)?have(.+)\\s*", std::regex::icase);
std::regex re_create_table("\\s*create\\s+table\\s+[A-Za-z0-9]+\\s*\\(\\s*[A-Za-z_]+\\s+(int|char\\s*\\(\\s*[0-9]+\\s*\\))(\\s+key)?(\\s*,\\s*[A-Za-z_]+\\s+(int|char\\s*\\(\\s*[0-9]+\\s*\\))(\\s+key)?\\s*)*\\s*\\)\\s*;?$", std::regex::icase);

std::regex re_select("\\s*select\\s+(.+)?\\s+from\\s+(.+)?\\s*;\\s*", std::regex::icase);
std::regex re_select_where("\\s*select\\s+(.+)?\\s+from\\s+(.+)?\\s+where\\s+(.+)?\\s*;\\s*", std::regex::icase);

std::regex re_join("(.+)\\.[a-zA-Z].*[=><]\\s*(.+)\\.[a-zA-Z].*"); //condition requirement: table.column
std::regex re_table("(.+)\\.[a-zA-Z].*");                          //condition requirement: table.column op value

std::regex re_col("(.+)?\\.(.+)?\\s*[=><].+");  
// std::regex re_create_table("\\s*create\\s+table\\s+([^\\(]+)?\\s*\\((.+)\\)\\s*;\\s*");

std::string PORT = ":" + std::to_string(FLAGS_port_server);


void trim(std::string &s)
{
    if (s.empty())
        return;
    s.erase(0, s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
}

void str_split(std::string s, std::vector<std::string> &result, std::string delimiter = ",")
{
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos)
    {
        token = s.substr(0, pos);
        trim(token);
        result.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    if (s.length())
    {
        trim(s);
        result.push_back(s);
    }
}

bool is_num(std::string &s){
    if(s.length()==0)   return false;
    if(s[0]!='+' && s[0]!='-' && !isdigit(s[0]))    return false;
    for(int i=1;i<s.length();i++){
        if(!isdigit(s[i]))  return false;
    }
    return true;
}

int str2int(std::string s){
    std::stringstream ss;
    int n;
    ss<<s;
    ss>>n;
    return n;
}

bool conflict(std::string sp, std::string frag_conds)
{
    std::vector<std::string> sp_vector;
    std::vector<std::string> frag_vector;
    str_split(sp, sp_vector, "and");
    str_split(frag_conds, frag_vector, "and");
    bool iS_conflict = false;
    for (auto cond : sp_vector)
    {
        cond.erase(0, cond.find_first_of(".") + 1);
        std::string col, val;
        std::string left, right; //range
        bool left_eq = false;
        bool right_eq = false;
        auto pos = cond.find_first_of("=><");
        col = cond.substr(0, pos);
        if (cond[pos] == '=')
        {
            val = cond.substr(pos + 1);
            left = right = val;
            left_eq = true;
            right_eq = true;
        }
        else if (cond[pos] == '<')
        {
            if (cond[pos + 1] == '=')
            {
                val = cond.substr(pos + 2);
                left = "min";
                right = val;
                right_eq = true;
            }
            else
            {
                val = cond.substr(pos + 1);
                left = "min";
                right = val;
            }
        }
        else if (cond[pos] == '>')
        {
            if (cond[pos + 1] == '=')
            {
                val = cond.substr(pos + 2);
                left = val;
                right = "max";
                left_eq = true;
            }
            else
            {
                val = cond.substr(pos + 1);
                left = val;
                right = "max";
            }
        }
        trim(left);
        trim(right);
        trim(col);
        for (auto frag : frag_vector)
        {
            std::string frag_col, frag_val;
            auto pos = frag.find_first_of("=><");
            frag_col = frag.substr(0, pos);
            trim(frag_col);
            if (frag_col == col)
            {
                if (frag[pos] == '=')
                {
                    frag_val = frag.substr(pos + 1);
                    trim(frag_val);
                    if (is_num(frag_val))
                    {
                        int cur = str2int(frag_val);
                        if (left == "min")
                        {
                            //should be a integer
                            int right_val = str2int(right);
                            if (cur > right_val)
                                return true;
                            if (cur == right_val && !right_eq)
                                return true;
                        }
                        else if (right == "max")
                        {
                            int left_val = str2int(left);
                            if (cur < left_val)
                                return true;
                            if (cur == left_val && !left_eq)
                                return true;
                        }
                        else
                        {
                            //left should eq right
                            if (left != right)
                                return true;
                            int lr_val = str2int(left);
                            if (lr_val != cur)
                                return true;
                        }
                    }
                    else
                    {
                        if (frag_val != val)
                            return true;
                    }
                }
                else if (frag[pos] == '<') //must be integer
                {
                    if (frag[pos + 1] == '=')
                    {
                        frag_val = frag.substr(pos + 2);
                        trim(frag_val);
                        int cur_val = str2int(frag_val);
                        if (right == "max")
                        {
                            int left_val = str2int(left);
                            if (left_val > cur_val)
                                return true;
                            if (left_val == cur_val)
                            {
                                if (!left_eq)
                                    return true;
                            }
                        }
                        if (left == right)
                        {
                            int lr_val = str2int(left);
                            if (lr_val > cur_val)
                                return true;
                        }
                    }
                    else
                    {
                        frag_val = frag.substr(pos + 2);
                        trim(frag_val);
                        int cur_val = str2int(frag_val);
                        if (right == "max")
                        {
                            int left_val = str2int(left);
                            if (left_val >= cur_val)
                                return true;
                        }
                        if (left == right)
                        {
                            int lr_val = str2int(left);
                            if (lr_val >= cur_val)
                                return true;
                        }
                    }
                }
                else if (frag[pos] == '>')
                {
                    if (frag[pos + 1] == '=')
                    {
                        frag_val = frag.substr(pos + 2);
                        trim(frag_val);
                        int cur_val = str2int(frag_val);
                        if (left == "min")
                        {
                            int right_val = str2int(right);
                            if (right_val < cur_val)
                                return true;
                            if (right_val == cur_val)
                            {
                                if (!right_eq)
                                    return true;
                            }
                        }
                        if (left == right)
                        {
                            int lr_val = str2int(left);
                            if (lr_val < cur_val)
                                return true;
                        }
                    }
                    else
                    {
                        frag_val = frag.substr(pos + 2);
                        trim(frag_val);
                        int cur_val = str2int(frag_val);
                        if (left == "min")
                        {
                            int right_val = str2int(right);
                            if (right_val <= cur_val)
                                return true;
                        }
                        if (left == right)
                        {
                            int lr_val = str2int(left);
                            if (lr_val <= cur_val)
                                return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}


std::string get_insert_site(std::string tablename, std::vector<std::string> columns, std::vector<std::string> values){
    std::string frag_table = read_kv_from_etcd(tablename);
    TableMeta tablemeta(nlohmann::json::parse(frag_table));
    std::vector<FragmentMeta> fragments = tablemeta.get_fragments();
    int n= columns.size();
    std::string cond = tablename + "." + columns[0] + "=" + values[0];
    for(int i=1;i<n;i++){
        cond+=" and ";
        cond+= tablename + "." + columns[i] + "=" + values[i];
    }
    for(auto frag:fragments){
        std::string frag_cond = frag.get_frag_predicate();
        // LOG(INFO) << cond << " " << frag_cond;
        if(conflict(cond,frag_cond)){
            continue;
        }
        return frag.get_frag_site_address();
    }
}

std::string parsesql(std::string input) {
    std::smatch base_match;
    std::string select_predicate;
    std::string join_predicate;
    std::string project_predicate;
    if (std::regex_match(input, base_match, re_select_where))
    {
        std::vector<std::string> columns;
        std::vector<std::string> tables;
        std::vector<std::string> condition_or;
        std::vector<std::vector<std::string>> condition_and;
        std::string column_str;
        std::string table_str;
        std::string where_str;
        if (base_match.size() >= 4)
        {
            column_str = base_match[1].str();
            table_str = base_match[2].str();
            where_str = base_match[3].str();
            str_split(column_str, columns);
            str_split(table_str, tables);
            str_split(where_str, condition_or, " or ");
            for (auto cond : condition_or)
            {
                std::vector<std::string> cond_and;
                str_split(cond, cond_and, " and ");
                condition_and.push_back(cond_and);
            }
        }
        std::unordered_map<std::string, std::string> table2conds;
        std::unordered_map<std::string, std::string> col2conds;
        for (auto table_name : tables)
        {
            table2conds[table_name] = "";
        }
        if (condition_and.size() == 1)
        {
            //no or, all and
            auto and_conds = condition_and[0];
            std::unordered_map<std::string, std::string> tablename_map; //real table name -> tmp table name
            
            for(auto cond : and_conds){
                if (!std::regex_match(cond, base_match, re_join)){
                    if(std::regex_match(cond, base_match, re_col)){
                        std::string t_name = base_match[1].str();
                        trim(t_name);
                        std::string col_name = t_name + "_" + base_match[2].str();
                        cond.replace(cond.find("."), 1, "." + t_name + "_");
                        col2conds[col_name] += cond + " and ";
                    }
                }
            }
            for(auto & it:col2conds){
                std::string tmp = it.second;
                if(tmp.length() > 5){
                    tmp.erase(tmp.length()-5,5);
                }
                it.second = tmp;
            }

            for (auto cond : and_conds)
            {
                if (!std::regex_match(cond, base_match, re_join))
                {
                    if (std::regex_match(cond, base_match, re_table))
                    {
                        if (base_match.size() >= 2)
                        {
                            std::string cur_table_name = base_match[1].str();
                            if (table2conds.find(cur_table_name) == table2conds.end())
                            {
                                //syntax error
                            }
                            cond.replace(cond.find("."), 1, "." + cur_table_name + "_");
                            table2conds[cur_table_name] += cond + " and ";
                        }
                    }
                }
            }
            for (auto table_name : tables)
            {
                if (table2conds[table_name].length() > 5)
                    table2conds[table_name].erase(table2conds[table_name].length() - 5, 5);
            }
            if (tables.size() == 1)
            {
                //one table,no join
                std::string tablename = tables[0];
                std::string project_predicate = columns[0];
                if (project_predicate != "*")
                {
                    for (auto &s : columns)
                    {
                        s.replace(s.find(tablename), tablename.length(), "tmp_table");
                        s.replace(s.find("."), 1, "." + tablename + "_");
                    }
                    project_predicate = columns[0];
                    for (int i = 1; i < columns.size(); i++)
                    {
                        project_predicate += ", " + columns[i];
                    }
                }

                Node *node = new Node(0, "", "", project_predicate, NODE_NOT_LEAF, false);
                std::map<std::string, std::string> node_map;
                node_map["tmp_table"] = "127.0.0.1" + PORT;
                node->set_children(node_map);
                // write_to_etcd("root_node", node->convert_to_json());

                // Node *child = new Node(0, table2conds[tablename], "", "", NODE_LEAF, false);
                // std::map<std::string, std::string> child_map;
                // child_map[tablename] = "127.0.0.1" + PORT;
                // child->set_children(child_map);
                // write_to_etcd("tmp_table", child->convert_to_json());

                std::string frag_table = read_kv_from_etcd(tablename);
                TableMeta tablemeta(nlohmann::json::parse(frag_table));

                std::vector<FragmentMeta> fragments = tablemeta.get_fragments();
                bool is_horizontal = false;
                if (fragments.size())
                {
                    auto fragment = fragments[0];
                    if (fragment.frag_is_horizontal())
                    {
                        is_horizontal = true;
                    }
                }
                std::string sp = table2conds[tablename];
                if (is_horizontal)
                {
                    Node *child = new Node(0, "", "", "", NODE_NOT_LEAF, true);
                    std::map<std::string, std::string> child_map;
                    // child_map[tablename] = "127.0.0.1" + PORT;
                    // child->set_children(child_map);
                    
                    int idx = 1;
                    int count = 0;
                    for (auto fragment : fragments)
                    {
                        std::string conds = fragment.get_frag_predicate();
                        if(conflict(sp,conds)){
                            continue;
                        }
                        count++;
                    }
                    if(count == 1){
                        for (auto fragment : fragments)
                        {
                            std::string conds = fragment.get_frag_predicate();
                            if(conflict(sp,conds)){
                                continue;
                            }
                            Node *frag = new Node(0, sp, "", "", NODE_LEAF, false);
                            std::map<std::string, std::string> frag_map;
                            frag_map[fragment.get_frag_table_name()]="127.0.0.1" + PORT;
                            frag->set_children(frag_map);
                            node->add_children("tmp_table",fragment.get_frag_site_address());
                            write_to_etcd("tmp_table", frag->convert_to_json());
                        }
                    }
                    else{
                        for (auto fragment : fragments)
                        {
                            std::string conds = fragment.get_frag_predicate();
                            if(conflict(sp,conds)){
                                continue;
                            }
                            Node *frag = new Node(0, sp, "", "", NODE_LEAF, false);
                            std::map<std::string, std::string> frag_map;
                            frag_map[fragment.get_frag_table_name()]="127.0.0.1" + PORT;
                            frag->set_children(frag_map);
                            std::string cur_node_name = "tmp_table_"+std::to_string(idx);
                            idx++;
                            child_map[cur_node_name]=fragment.get_frag_site_address();
                            write_to_etcd(cur_node_name, frag->convert_to_json());
                        }

                        child->set_children(child_map);
                        write_to_etcd("tmp_table", child->convert_to_json());
                    }
                    
                }
                else{
                    int vertical_counts = fragments.size();
                    if(vertical_counts == 2){
                        std::string jp = "";
                        std::string frag_col = fragments[0].get_frag_field().get_field_name();
                        jp = "child_tmp1."+frag_col+ "=" + "child_tmp2."+frag_col;
                        Node *child = new Node(0, "", jp, "", NODE_NOT_LEAF, false);
                        std::map<std::string, std::string> child_map;
                        auto site1 = fragments[0].get_frag_site_address();
                        auto site2 = fragments[1].get_frag_site_address();
                        child_map["child_tmp1"] = site1;
                        child_map["child_tmp2"] = site2;
                        child->set_children(child_map);
                        write_to_etcd("tmp_table", child->convert_to_json());

                        std::string v1_ss = "";
                        std::string v2_ss = "";
                        for(auto it:col2conds){
                            LOG(INFO) << it.first << " : " << it.second;
                            auto col_name = it.first;
                            trim(col_name);
                            auto fields1 = fragments[0].get_fields();
                            auto fields2 = fragments[1].get_fields();
                            std::vector<std::string> fields_name1;
                            std::vector<std::string> fields_name2;

                            for(auto field : fields1) {
                                fields_name1.push_back(field.get_field_name());
                                // LOG(INFO) << "field1 push " << field.get_field_name();
                            }
                            for(auto field : fields2) {
                                fields_name2.push_back(field.get_field_name());
                                // LOG(INFO) << "field2 push " << field.get_field_name();
                            }

                            auto it1 = std::find(fields_name1.begin(), fields_name1.end(),col_name);
                            if(it1 != fields_name1.end()){
                                // LOG(INFO) << "field1 find " << col_name;
                                v1_ss += it.second + " and ";
                            }
                            auto it2 = std::find(fields_name2.begin(), fields_name2.end(), col_name);
                            if(it2 != fields_name2.end()){
                                // LOG(INFO) << "field2 find " << col_name;
                                v2_ss += it.second + " and ";
                            }
                        }
                        // LOG(INFO) << "v1" << v1_ss;
                        // LOG(INFO) << "v2" << v2_ss;
                        if(v1_ss.length() >=5){
                            v1_ss.erase(v1_ss.length()-5,5);
                        }
                        if(v2_ss.length() >=5){
                            v2_ss.erase(v2_ss.length()-5,5);
                        }
                        // LOG(INFO) << "v1" << v1_ss;
                        // LOG(INFO) << "v2" << v2_ss;
                        Node * child_tmp1 = new Node(0, v1_ss, "", "", NODE_LEAF, false);
                        Node * child_tmp2 = new Node(0, v2_ss, "", "", NODE_LEAF, false);
                        // LOG(INFO) << "child select predicate " << child_tmp1->get_select_predicate() << " " << child_tmp2->get_select_predicate();
                        std::map<std::string,std::string> tmp1_map;
                        std::map<std::string,std::string> tmp2_map;
                        tmp1_map[fragments[0].get_frag_table_name()] = "127.0.0.1" + PORT;
                        tmp2_map[fragments[1].get_frag_table_name()] = "127.0.0.1" + PORT;
                        child_tmp1->set_children(tmp1_map);
                        child_tmp2->set_children(tmp2_map);
                        write_to_etcd("child_tmp1", child_tmp1->convert_to_json());
                        write_to_etcd("child_tmp2", child_tmp2->convert_to_json());
                    }
                }

                write_to_etcd("root_node", node->convert_to_json());
                return "root_node";
            }
            else if (tables.size() == 2)
            {
                bool equal_join = false;
                std::string project_predicate = columns[0];
                tablename_map[tables[0]] = "tmp_table1";
                tablename_map[tables[1]] = "tmp_table2";
                if (project_predicate != "*")
                {
                    for (auto &s : columns)
                    {
                        std::string cur_tablename = s.substr(0, s.find("."));
                        s.replace(s.find(cur_tablename), cur_tablename.length(), tablename_map[cur_tablename]);
                        s.replace(s.find("."), 1, "." + cur_tablename + "_");
                    }
                    project_predicate = columns[0];
                    for (int i = 1; i < columns.size(); i++)
                    {
                        project_predicate += ", " + columns[i];
                    }
                }
                for (auto cond : and_conds)
                {
                    if (std::regex_match(cond, base_match, re_join))
                    {
                        equal_join = true;
                        std::string table_name1 = base_match[1].str();
                        std::string table_name2 = base_match[2].str();

                        std::string join_predicate = cond;
                        join_predicate.replace(join_predicate.find(table_name1), table_name1.length(), tablename_map[table_name1]);
                        join_predicate.replace(join_predicate.rfind(table_name2), table_name2.length(), tablename_map[table_name2]);
                        join_predicate.replace(join_predicate.find_first_of("."), 1, "." + table_name1 + "_");
                        join_predicate.replace(join_predicate.find_last_of("."), 1, "." + table_name2 + "_");

                        Node *node = new Node(0, "", join_predicate, project_predicate, NODE_NOT_LEAF, false);
                        std::map<std::string, std::string> node_map;
                        node_map[tablename_map[table_name1]] = "127.0.0.1" + PORT;
                        node_map[tablename_map[table_name2]] = "127.0.0.1" + PORT;
                        node->set_children(node_map);

                        std::string frag_table = read_kv_from_etcd(table_name1);
                        TableMeta tablemeta1(nlohmann::json::parse(frag_table));
                        std::vector<FragmentMeta> fragments = tablemeta1.get_fragments();

                        bool is_horizontal = false;
                        if(fragments.size())
                        {
                            auto fragment = fragments[0];
                            if (fragment.frag_is_horizontal())
                            {
                                is_horizontal = true;
                            }
                        }
                        std::string sp = table2conds[table_name1];
                        if(is_horizontal)
                        {
                            Node *child = new Node(0, "", "", "", NODE_NOT_LEAF, true);
                            std::map<std::string,std::string> child_map;
                            int idx = 1;
                            int count = 0;
                            for (auto fragment : fragments)
                            {
                                std::string conds = fragment.get_frag_predicate();
                                if(conflict(sp,conds))
                                {
                                    continue;
                                }
                                count++;
                            }
                            if(count == 1)
                            {
                                for (auto fragment : fragments)
                                {
                                    std::string conds = fragment.get_frag_predicate();
                                    if(conflict(sp,conds))
                                    {
                                        continue;
                                    }
                                    Node *frag = new Node(0, sp, "", "", NODE_LEAF, false);
                                    std::map<std::string, std::string> frag_map;
                                    frag_map[fragment.get_frag_table_name()]="127.0.0.1" + PORT;
                                    frag->set_children(frag_map);
                                    node->add_children("tmp_table1",fragment.get_frag_site_address());
                                    write_to_etcd("tmp_table1", frag->convert_to_json());
                                }
                            }
                            else
                            {
                                for (auto fragment : fragments)
                                {
                                    std::string conds = fragment.get_frag_predicate();
                                    if(conflict(sp,conds)){
                                        continue;
                                    }
                                    Node *frag = new Node(0, sp, "", "", NODE_LEAF, false);
                                    std::map<std::string, std::string> frag_map;
                                    frag_map[fragment.get_frag_table_name()]="127.0.0.1" + PORT;
                                    frag->set_children(frag_map);
                                    std::string cur_node_name = "tmp_table1_"+std::to_string(idx);
                                    idx++;
                                    child_map[cur_node_name]=fragment.get_frag_site_address();
                                    write_to_etcd(cur_node_name, frag->convert_to_json());
                                }
                                child->set_children(child_map);
                                write_to_etcd("tmp_table1", child->convert_to_json());
                            }
                        }
                        else
                        {
                            int vertical_counts = fragments.size();
                            if(vertical_counts == 2)
                            {
                                std::string jp = "";
                                std::string frag_col = fragments[0].get_frag_field().get_field_name();
                                jp = "tmp_table1_1."+frag_col+ "=" + "tmp_table1_2."+frag_col;
                                Node *child = new Node(0, "", jp, "", NODE_NOT_LEAF, false);
                                std::map<std::string, std::string> child_map;
                                auto site1 = fragments[0].get_frag_site_address();
                                auto site2 = fragments[1].get_frag_site_address();
                                child_map["tmp_table1_1"] = site1;
                                child_map["tmp_table1_2"] = site2;
                                child->set_children(child_map);
                                write_to_etcd("tmp_table1", child->convert_to_json());

                                std::string v1_ss = "";
                                std::string v2_ss = "";

                                for(auto it:col2conds)
                                {
                                    auto col_name = it.first;
                                    trim(col_name);
                                    auto fields1 = fragments[0].get_fields();
                                    auto fields2 = fragments[1].get_fields();
                                    std::vector<std::string> fields_name1;
                                    std::vector<std::string> fields_name2;

                                    for(auto field : fields1) {
                                        fields_name1.push_back(field.get_field_name());
                                    }
                                    for(auto field : fields2) {
                                        fields_name2.push_back(field.get_field_name());
                                    }
                                    auto it1 = std::find(fields_name1.begin(), fields_name1.end(),col_name);
                                    if(it1 != fields_name1.end()){
                                        v1_ss += it.second + " and ";
                                    }
                                    auto it2 = std::find(fields_name2.begin(), fields_name2.end(), col_name);
                                    if(it2 != fields_name2.end()){
                                        v2_ss += it.second + " and ";
                                    }
                                    
                                }
                                if(v1_ss.length() >=5){
                                    v1_ss.erase(v1_ss.length()-5,5);
                                }
                                if(v2_ss.length() >=5){
                                    v2_ss.erase(v2_ss.length()-5,5);
                                }
                                Node * child_tmp1 = new Node(0, v1_ss, "", "", NODE_LEAF, false);
                                Node * child_tmp2 = new Node(0, v2_ss, "", "", NODE_LEAF, false);
                                std::map<std::string,std::string> tmp1_map;
                                std::map<std::string,std::string> tmp2_map;
                                tmp1_map[fragments[0].get_frag_table_name()] = "127.0.0.1" + PORT;
                                tmp2_map[fragments[1].get_frag_table_name()] = "127.0.0.1" + PORT;
                                child_tmp1->set_children(tmp1_map);
                                child_tmp2->set_children(tmp2_map);
                                write_to_etcd("tmp_table1_1", child_tmp1->convert_to_json());
                                write_to_etcd("tmp_table1_2", child_tmp2->convert_to_json());

                            }
                        }

                        frag_table = read_kv_from_etcd(table_name2);
                        TableMeta tablemeta2(nlohmann::json::parse(frag_table));
                        fragments = tablemeta2.get_fragments();

                        is_horizontal = false;
                        if(fragments.size())
                        {
                            auto fragment = fragments[0];
                            if (fragment.frag_is_horizontal())
                            {
                                is_horizontal = true;
                            }
                        }
                        sp = table2conds[table_name2];
                        if(is_horizontal)
                        {
                            Node *child = new Node(0, "", "", "", NODE_NOT_LEAF, true);
                            std::map<std::string,std::string> child_map;
                            int idx = 1;
                            int count = 0;
                            for (auto fragment : fragments)
                            {
                                std::string conds = fragment.get_frag_predicate();
                                if(conflict(sp,conds))
                                {
                                    continue;
                                }
                                count++;
                            }
                            if(count == 1)
                            {
                                for (auto fragment : fragments)
                                {
                                    std::string conds = fragment.get_frag_predicate();
                                    if(conflict(sp,conds))
                                    {
                                        continue;
                                    }
                                    Node *frag = new Node(0, sp, "", "", NODE_LEAF, false);
                                    std::map<std::string, std::string> frag_map;
                                    frag_map[fragment.get_frag_table_name()]="127.0.0.1" + PORT;
                                    frag->set_children(frag_map);
                                    node->add_children("tmp_table2",fragment.get_frag_site_address());
                                    write_to_etcd("tmp_table2", frag->convert_to_json());
                                }
                            }
                            else
                            {
                                for (auto fragment : fragments)
                                {
                                    std::string conds = fragment.get_frag_predicate();
                                    if(conflict(sp,conds)){
                                        continue;
                                    }
                                    Node *frag = new Node(0, sp, "", "", NODE_LEAF, false);
                                    std::map<std::string, std::string> frag_map;
                                    frag_map[fragment.get_frag_table_name()]="127.0.0.1" + PORT;
                                    frag->set_children(frag_map);
                                    std::string cur_node_name = "tmp_table2_"+std::to_string(idx);
                                    idx++;
                                    child_map[cur_node_name]=fragment.get_frag_site_address();
                                    write_to_etcd(cur_node_name, frag->convert_to_json());
                                }
                                child->set_children(child_map);
                                write_to_etcd("tmp_table2", child->convert_to_json());
                            }
                        }
                        else
                        {
                            int vertical_counts = fragments.size();
                            if(vertical_counts == 2)
                            {
                                std::string jp = "";
                                std::string frag_col = fragments[0].get_frag_field().get_field_name();
                                jp = "tmp_table2_1."+frag_col+ "=" + "tmp_table2_2."+frag_col;
                                Node *child = new Node(0, "", jp, "", NODE_NOT_LEAF, false);
                                std::map<std::string, std::string> child_map;
                                auto site1 = fragments[0].get_frag_site_address();
                                auto site2 = fragments[1].get_frag_site_address();
                                child_map["tmp_table2_1"] = site1;
                                child_map["tmp_table2_2"] = site2;
                                child->set_children(child_map);
                                write_to_etcd("tmp_table2", child->convert_to_json());

                                std::string v1_ss = "";
                                std::string v2_ss = "";

                                for(auto it:col2conds)
                                {
                                    auto col_name = it.first;
                                    trim(col_name);
                                    auto fields1 = fragments[0].get_fields();
                                    auto fields2 = fragments[1].get_fields();
                                    std::vector<std::string> fields_name1;
                                    std::vector<std::string> fields_name2;

                                    for(auto field : fields1) {
                                        fields_name1.push_back(field.get_field_name());
                                    }
                                    for(auto field : fields2) {
                                        fields_name2.push_back(field.get_field_name());
                                    }
                                    auto it1 = std::find(fields_name1.begin(), fields_name1.end(),col_name);
                                    if(it1 != fields_name1.end()){
                                        v1_ss += it.second + " and ";
                                    }
                                    auto it2 = std::find(fields_name2.begin(), fields_name2.end(), col_name);
                                    if(it2 != fields_name2.end()){
                                        v2_ss += it.second + " and ";
                                    }
                                    
                                }
                                if(v1_ss.length() >=5){
                                    v1_ss.erase(v1_ss.length()-5,5);
                                }
                                if(v2_ss.length() >=5){
                                    v2_ss.erase(v2_ss.length()-5,5);
                                }
                                Node * child_tmp1 = new Node(0, v1_ss, "", "", NODE_LEAF, false);
                                Node * child_tmp2 = new Node(0, v2_ss, "", "", NODE_LEAF, false);
                                std::map<std::string,std::string> tmp1_map;
                                std::map<std::string,std::string> tmp2_map;
                                tmp1_map[fragments[0].get_frag_table_name()] = "127.0.0.1" + PORT;
                                tmp2_map[fragments[1].get_frag_table_name()] = "127.0.0.1" + PORT;
                                child_tmp1->set_children(tmp1_map);
                                child_tmp2->set_children(tmp2_map);
                                write_to_etcd("tmp_table2_1", child_tmp1->convert_to_json());
                                write_to_etcd("tmp_table2_2", child_tmp2->convert_to_json());

                            }
                        }

                        write_to_etcd("root_node", node->convert_to_json());
                        return "root_node";
                    }
                }
                if (!equal_join)
                {
                    //not equal join
                    Node *node = new Node(0, "", "", project_predicate, NODE_NOT_LEAF, false);
                    std::string table_name1 = tables[0];
                    std::string table_name2 = tables[1];
                    Node *child1 = new Node(0, table2conds[tables[0]], "", "", NODE_LEAF, false);
                    Node *child2 = new Node(0, table2conds[tables[1]], "", "", NODE_LEAF, false);

                    std::map<std::string, std::string> child1_map;
                    child1_map[table_name1] = "127.0.0.1" + PORT;
                    child1->set_children(child1_map);

                    std::map<std::string, std::string> child2_map;
                    child2_map[table_name2] = "127.0.0.1" + PORT;
                    child2->set_children(child2_map);

                    std::map<std::string, std::string> node_map;
                    node_map[tablename_map[tables[0]]] = "127.0.0.1" + PORT;
                    node_map[tablename_map[tables[1]]] = "127.0.0.1" + PORT;
                    node->set_children(node_map);

                    write_to_etcd("root_node", node->convert_to_json());
                    write_to_etcd("tmp_table1", child1->convert_to_json());
                    write_to_etcd("tmp_table2", child2->convert_to_json());

                    return "root_node";
                }
            }
            else if (tables.size() == 3)
            {
                //adjust tables order
                bool cur_12_can_join = false;
                for (auto cond : and_conds)
                {
                    if (std::regex_match(cond, base_match, re_join))
                    {
                        std::string table_name1 = base_match[1].str();
                        std::string table_name2 = base_match[2].str();
                        if ((table_name1 == tables[0] && table_name2 == tables[1]) || (table_name1 == tables[1] && table_name2 == tables[0]))
                        {
                            cur_12_can_join = true;
                            break;
                        }
                    }
                }
                if (!cur_12_can_join)
                {
                    auto tmp = tables[1];
                    tables[1] = tables[2];
                    tables[2] = tmp;
                }

                for (int i = 0; i < tables.size(); i++)
                {
                    tablename_map[tables[i]] = "tmp_table" + std::to_string(i + 1);
                }
                // Node *leaf1 = new Node(0, table2conds[tables[0]], "", "", NODE_LEAF, false);
                // std::map<std::string, std::string> leaf1_map;
                // leaf1_map[tables[0]] = "127.0.0.1" + PORT;
                // leaf1->set_children(leaf1_map);

                std::string frag_table = read_kv_from_etcd(tables[0]);
                TableMeta tablemeta1(nlohmann::json::parse(frag_table));
                std::vector<FragmentMeta> fragments = tablemeta1.get_fragments();

                bool is_horizontal = false;
                if(fragments.size())
                {
                    auto fragment = fragments[0];
                    if (fragment.frag_is_horizontal())
                    {
                        is_horizontal = true;
                    }
                }
                std::string sp = table2conds[tables[0]];

                if(is_horizontal)
                {
                    Node *child = new Node(0, "", "", "", NODE_NOT_LEAF, true);
                    std::map<std::string,std::string> child_map;
                    int idx = 1;
                    for (auto fragment : fragments)
                    {
                        std::string conds = fragment.get_frag_predicate();
                        if(conflict(sp,conds)){
                            continue;
                        }
                        Node *frag = new Node(0, sp, "", "", NODE_LEAF, false);
                        std::map<std::string, std::string> frag_map;
                        frag_map[fragment.get_frag_table_name()]="127.0.0.1" + PORT;
                        frag->set_children(frag_map);
                        std::string cur_node_name = "tmp_table1_"+std::to_string(idx);
                        idx++;
                        child_map[cur_node_name]=fragment.get_frag_site_address();
                        write_to_etcd(cur_node_name, frag->convert_to_json());
                    }
                    child->set_children(child_map);
                    write_to_etcd("tmp_table1", child->convert_to_json());
                }
                else
                {
                    int vertical_counts = fragments.size();
                    if(vertical_counts == 2)
                    {
                        std::string jp = "";
                        std::string frag_col = fragments[0].get_frag_field().get_field_name();
                        jp = "tmp_table1_1."+frag_col+ "=" + "tmp_table1_2."+frag_col;
                        Node *child = new Node(0, "", jp, "", NODE_NOT_LEAF, false);
                        std::map<std::string, std::string> child_map;
                        auto site1 = fragments[0].get_frag_site_address();
                        auto site2 = fragments[1].get_frag_site_address();
                        child_map["tmp_table1_1"] = site1;
                        child_map["tmp_table1_2"] = site2;
                        child->set_children(child_map);
                        write_to_etcd("tmp_table1", child->convert_to_json());

                        std::string v1_ss = "";
                        std::string v2_ss = "";

                        for(auto it:col2conds)
                        {
                            auto col_name = it.first;
                            trim(col_name);
                            auto fields1 = fragments[0].get_fields();
                            auto fields2 = fragments[1].get_fields();
                            std::vector<std::string> fields_name1;
                            std::vector<std::string> fields_name2;

                            for(auto field : fields1) {
                                fields_name1.push_back(field.get_field_name());
                            }
                            for(auto field : fields2) {
                                fields_name2.push_back(field.get_field_name());
                            }
                            auto it1 = std::find(fields_name1.begin(), fields_name1.end(),col_name);
                            if(it1 != fields_name1.end()){
                                v1_ss += it.second + " and ";
                            }
                            auto it2 = std::find(fields_name2.begin(), fields_name2.end(), col_name);
                            if(it2 != fields_name2.end()){
                                v2_ss += it.second + " and ";
                            }
                                    
                        }
                        if(v1_ss.length() >=5){
                            v1_ss.erase(v1_ss.length()-5,5);
                        }
                        if(v2_ss.length() >=5){
                            v2_ss.erase(v2_ss.length()-5,5);
                        }
                        Node * child_tmp1 = new Node(0, v1_ss, "", "", NODE_LEAF, false);
                        Node * child_tmp2 = new Node(0, v2_ss, "", "", NODE_LEAF, false);
                        std::map<std::string,std::string> tmp1_map;
                        std::map<std::string,std::string> tmp2_map;
                        tmp1_map[fragments[0].get_frag_table_name()] = "127.0.0.1" + PORT;
                        tmp2_map[fragments[1].get_frag_table_name()] = "127.0.0.1" + PORT;
                        child_tmp1->set_children(tmp1_map);
                        child_tmp2->set_children(tmp2_map);
                        write_to_etcd("tmp_table1_1", child_tmp1->convert_to_json());
                        write_to_etcd("tmp_table1_2", child_tmp2->convert_to_json());
                    }
                }

                // Node *leaf2 = new Node(0, table2conds[tables[1]], "", "", NODE_LEAF, false);
                // std::map<std::string, std::string> leaf2_map;
                // leaf2_map[tables[1]] = "127.0.0.1" + PORT;
                // leaf2->set_children(leaf2_map);
                frag_table = read_kv_from_etcd(tables[1]);
                TableMeta tablemeta2(nlohmann::json::parse(frag_table));
                fragments = tablemeta2.get_fragments();

                is_horizontal = false;
                if(fragments.size())
                {
                    auto fragment = fragments[0];
                    if (fragment.frag_is_horizontal())
                    {
                        is_horizontal = true;
                    }
                }
                sp = table2conds[tables[1]];

                if(is_horizontal)
                {
                    Node *child = new Node(0, "", "", "", NODE_NOT_LEAF, true);
                    std::map<std::string,std::string> child_map;
                    int idx = 1;
                    for (auto fragment : fragments)
                    {
                        std::string conds = fragment.get_frag_predicate();
                        if(conflict(sp,conds)){
                            continue;
                        }
                        Node *frag = new Node(0, sp, "", "", NODE_LEAF, false);
                        std::map<std::string, std::string> frag_map;
                        frag_map[fragment.get_frag_table_name()]="127.0.0.1" + PORT;
                        frag->set_children(frag_map);
                        std::string cur_node_name = "tmp_table2_"+std::to_string(idx);
                        idx++;
                        child_map[cur_node_name]=fragment.get_frag_site_address();
                        write_to_etcd(cur_node_name, frag->convert_to_json());
                    }
                    child->set_children(child_map);
                    write_to_etcd("tmp_table2", child->convert_to_json());
                }
                else
                {
                    int vertical_counts = fragments.size();
                    if(vertical_counts == 2)
                    {
                        std::string jp = "";
                        std::string frag_col = fragments[0].get_frag_field().get_field_name();
                        jp = "tmp_table2_1."+frag_col+ "=" + "tmp_table2_2."+frag_col;
                        Node *child = new Node(0, "", jp, "", NODE_NOT_LEAF, false);
                        std::map<std::string, std::string> child_map;
                        auto site1 = fragments[0].get_frag_site_address();
                        auto site2 = fragments[1].get_frag_site_address();
                        child_map["tmp_table2_1"] = site1;
                        child_map["tmp_table2_2"] = site2;
                        child->set_children(child_map);
                        write_to_etcd("tmp_table2", child->convert_to_json());

                        std::string v1_ss = "";
                        std::string v2_ss = "";

                        for(auto it:col2conds)
                        {
                            auto col_name = it.first;
                            trim(col_name);
                            auto fields1 = fragments[0].get_fields();
                            auto fields2 = fragments[1].get_fields();
                            std::vector<std::string> fields_name1;
                            std::vector<std::string> fields_name2;

                            for(auto field : fields1) {
                                fields_name1.push_back(field.get_field_name());
                            }
                            for(auto field : fields2) {
                                fields_name2.push_back(field.get_field_name());
                            }
                            auto it1 = std::find(fields_name1.begin(), fields_name1.end(),col_name);
                            if(it1 != fields_name1.end()){
                                v1_ss += it.second + " and ";
                            }
                            auto it2 = std::find(fields_name2.begin(), fields_name2.end(), col_name);
                            if(it2 != fields_name2.end()){
                                v2_ss += it.second + " and ";
                            }
                                    
                        }
                        if(v1_ss.length() >=5){
                            v1_ss.erase(v1_ss.length()-5,5);
                        }
                        if(v2_ss.length() >=5){
                            v2_ss.erase(v2_ss.length()-5,5);
                        }
                        Node * child_tmp1 = new Node(0, v1_ss, "", "", NODE_LEAF, false);
                        Node * child_tmp2 = new Node(0, v2_ss, "", "", NODE_LEAF, false);
                        std::map<std::string,std::string> tmp1_map;
                        std::map<std::string,std::string> tmp2_map;
                        tmp1_map[fragments[0].get_frag_table_name()] = "127.0.0.1" + PORT;
                        tmp2_map[fragments[1].get_frag_table_name()] = "127.0.0.1" + PORT;
                        child_tmp1->set_children(tmp1_map);
                        child_tmp2->set_children(tmp2_map);
                        write_to_etcd("tmp_table2_1", child_tmp1->convert_to_json());
                        write_to_etcd("tmp_table2_2", child_tmp2->convert_to_json());
                    }
                }


                // Node *leaf3 = new Node(0, table2conds[tables[2]], "", "", NODE_LEAF, false);
                // std::map<std::string, std::string> leaf3_map;
                // leaf3_map[tables[2]] = "127.0.0.1" + PORT;
                // leaf3->set_children(leaf3_map);

                frag_table = read_kv_from_etcd(tables[2]);
                TableMeta tablemeta3(nlohmann::json::parse(frag_table));
                fragments = tablemeta3.get_fragments();

                is_horizontal = false;
                if(fragments.size())
                {
                    auto fragment = fragments[0];
                    if (fragment.frag_is_horizontal())
                    {
                        is_horizontal = true;
                    }
                }
                sp = table2conds[tables[2]];

                if(is_horizontal)
                {
                    Node *child = new Node(0, "", "", "", NODE_NOT_LEAF, true);
                    std::map<std::string,std::string> child_map;
                    int idx = 1;
                    for (auto fragment : fragments)
                    {
                        std::string conds = fragment.get_frag_predicate();
                        if(conflict(sp,conds)){
                            continue;
                        }
                        Node *frag = new Node(0, sp, "", "", NODE_LEAF, false);
                        std::map<std::string, std::string> frag_map;
                        frag_map[fragment.get_frag_table_name()]="127.0.0.1" + PORT;
                        frag->set_children(frag_map);
                        std::string cur_node_name = "tmp_table3_"+std::to_string(idx);
                        idx++;
                        child_map[cur_node_name]=fragment.get_frag_site_address();
                        write_to_etcd(cur_node_name, frag->convert_to_json());
                    }
                    child->set_children(child_map);
                    write_to_etcd("tmp_table3", child->convert_to_json());
                }
                else
                {
                    int vertical_counts = fragments.size();
                    if(vertical_counts == 2)
                    {
                        std::string jp = "";
                        std::string frag_col = fragments[0].get_frag_field().get_field_name();
                        jp = "tmp_table3_1."+frag_col+ "=" + "tmp_table3_2."+frag_col;
                        Node *child = new Node(0, "", jp, "", NODE_NOT_LEAF, false);
                        std::map<std::string, std::string> child_map;
                        auto site1 = fragments[0].get_frag_site_address();
                        auto site2 = fragments[1].get_frag_site_address();
                        child_map["tmp_table3_1"] = site1;
                        child_map["tmp_table3_2"] = site2;
                        child->set_children(child_map);
                        write_to_etcd("tmp_table3", child->convert_to_json());

                        std::string v1_ss = "";
                        std::string v2_ss = "";

                        for(auto it:col2conds)
                        {
                            auto col_name = it.first;
                            trim(col_name);
                            auto fields1 = fragments[0].get_fields();
                            auto fields2 = fragments[1].get_fields();
                            std::vector<std::string> fields_name1;
                            std::vector<std::string> fields_name2;

                            for(auto field : fields1) {
                                fields_name1.push_back(field.get_field_name());
                            }
                            for(auto field : fields2) {
                                fields_name2.push_back(field.get_field_name());
                            }
                            auto it1 = std::find(fields_name1.begin(), fields_name1.end(),col_name);
                            if(it1 != fields_name1.end()){
                                v1_ss += it.second + " and ";
                            }
                            auto it2 = std::find(fields_name2.begin(), fields_name2.end(), col_name);
                            if(it2 != fields_name2.end()){
                                v2_ss += it.second + " and ";
                            }
                                    
                        }
                        if(v1_ss.length() >=5){
                            v1_ss.erase(v1_ss.length()-5,5);
                        }
                        if(v2_ss.length() >=5){
                            v2_ss.erase(v2_ss.length()-5,5);
                        }
                        Node * child_tmp1 = new Node(0, v1_ss, "", "", NODE_LEAF, false);
                        Node * child_tmp2 = new Node(0, v2_ss, "", "", NODE_LEAF, false);
                        std::map<std::string,std::string> tmp1_map;
                        std::map<std::string,std::string> tmp2_map;
                        tmp1_map[fragments[0].get_frag_table_name()] = "127.0.0.1" + PORT;
                        tmp2_map[fragments[1].get_frag_table_name()] = "127.0.0.1" + PORT;
                        child_tmp1->set_children(tmp1_map);
                        child_tmp2->set_children(tmp2_map);
                        write_to_etcd("tmp_table3_1", child_tmp1->convert_to_json());
                        write_to_etcd("tmp_table3_2", child_tmp2->convert_to_json());
                    }
                }


                bool leaf12_eq_join = false;
                Node *tmp_table4;

                //consider *
                std::string tmp_table4_pp = "";
                // for (auto s : columns)
                // {
                //     std::string cur_tablename = s.substr(0, s.find("."));
                //     if (cur_tablename == tables[0] || cur_tablename == tables[1])
                //     {
                //         s.replace(s.find(cur_tablename), cur_tablename.length(), tablename_map[cur_tablename]);
                //         s.replace(s.find("."), 1, "." + cur_tablename + "_");
                //         tmp_table4_pp += s + ", ";
                //     }
                // }
                // tmp_table4_pp.erase(tmp_table4_pp.find_last_of(","));

                for (auto cond : and_conds)
                {
                    if (std::regex_match(cond, base_match, re_join))
                    {
                        std::string table_name1 = base_match[1].str();
                        std::string table_name2 = base_match[2].str();

                        if ((table_name1 == tables[0] && table_name2 == tables[1]) || (table_name1 == tables[1] && table_name2 == tables[0]))
                        {
                            leaf12_eq_join = true;
                            cond.replace(cond.find(table_name1), table_name1.length(), tablename_map[table_name1]);
                            cond.replace(cond.rfind(table_name2), table_name2.length(), tablename_map[table_name2]);
                            cond.replace(cond.find_first_of("."), 1, "." + table_name1 + "_");
                            cond.replace(cond.find_last_of("."), 1, "." + table_name2 + "_");
                            tmp_table4 = new Node(0, "", cond, tmp_table4_pp, NODE_NOT_LEAF, false);
                            break;
                        }
                    }
                }
                if (!leaf12_eq_join)
                {
                    tmp_table4 = new Node(0, "", "", tmp_table4_pp, NODE_NOT_LEAF, false);
                }
                std::map<std::string, std::string> tmp_table4_map;
                tmp_table4_map["tmp_table1"] = "127.0.0.1" + PORT; //leaf1
                tmp_table4_map["tmp_table2"] = "127.0.0.1" + PORT; //leaf2
                tmp_table4->set_children(tmp_table4_map);
                Node *node;
                bool leaf3_eq_join = false;
                std::string node_pp = "";
                for (auto s : columns)
                {
                    std::string cur_tablename = s.substr(0, s.find("."));
                    if (cur_tablename == tables[0] || cur_tablename == tables[1])
                    {
                        s.replace(s.find(cur_tablename), cur_tablename.length(), "tmp_table4");
                        s.replace(s.find("."), 1, "." + cur_tablename + "_");
                        node_pp += s + ", ";
                    }
                    else if (cur_tablename == tables[2])
                    {
                        s.replace(s.find(cur_tablename), cur_tablename.length(), tablename_map[cur_tablename]);
                        s.replace(s.find("."), 1, "." + cur_tablename + "_");
                        node_pp += s + ", ";
                    }
                }
                node_pp.erase(node_pp.find_last_of(","));
                for (auto cond : and_conds)
                {
                    if (std::regex_match(cond, base_match, re_join))
                    {
                        std::string table_name1 = base_match[1].str();
                        std::string table_name2 = base_match[2].str();

                        if (table_name1 == tables[2] || table_name2 == tables[2])
                        {
                            leaf3_eq_join = true;
                            std::string another_table;
                            if (table_name1 == tables[2])
                            {
                                another_table = table_name2;
                            }
                            else
                            {
                                another_table = table_name1;
                            }
                            cond.replace(cond.find(tables[2]), tables[2].length(), tablename_map[tables[2]]);
                            cond.replace(cond.find(another_table), another_table.length(), "tmp_table4");
                            cond.replace(cond.find_first_of("."), 1, "." + table_name1 + "_");
                            cond.replace(cond.find_last_of("."), 1, "." + table_name2 + "_");

                            node = new Node(0, "", cond, node_pp, NODE_NOT_LEAF, false);
                            break;
                        }
                    }
                }

                if (!leaf3_eq_join)
                {
                    node = new Node(0, "", "", node_pp, NODE_NOT_LEAF, false);
                }
                std::map<std::string, std::string> node_map;
                node_map["tmp_table4"] = "127.0.0.1" + PORT;
                node_map["tmp_table3"] = "127.0.0.1" + PORT;
                node->set_children(node_map);

                write_to_etcd("root_node", node->convert_to_json());
                write_to_etcd("tmp_table4", tmp_table4->convert_to_json());
                // write_to_etcd("tmp_table1", leaf1->convert_to_json());
                // write_to_etcd("tmp_table2", leaf2->convert_to_json());
                // write_to_etcd("tmp_table3", leaf3->convert_to_json());

                return "root_node";
            }
            else if (tables.size() == 4)
            {
                //adjust tables order
                bool cur_12_can_join = false;
                bool actually_12_can_join = false;
                for (auto cond : and_conds)
                {
                    if (std::regex_match(cond, base_match, re_join))
                    {
                        std::string table_name1 = base_match[1].str();
                        std::string table_name2 = base_match[2].str();
                        if ((table_name1 == tables[0] && table_name2 == tables[1]) || (table_name1 == tables[1] && table_name2 == tables[0]))
                        {
                            cur_12_can_join = true;
                            break;
                        }
                    }
                }
                if (cur_12_can_join)
                {
                    for (auto cond : and_conds)
                    {
                        if (std::regex_match(cond, base_match, re_join))
                        {
                            std::string table_name1 = base_match[1].str();
                            std::string table_name2 = base_match[2].str();
                            if ((table_name1 == tables[2] && table_name2 == tables[3]) || (table_name1 == tables[3] && table_name2 == tables[2]))
                            {
                                actually_12_can_join = true;
                            }
                        }
                    }
                }
                if (!actually_12_can_join)
                {
                    bool cur_13_can_join = false;
                    bool actually_13_can_join = false;
                    for (auto cond : and_conds)
                    {
                        if (std::regex_match(cond, base_match, re_join))
                        {
                            std::string table_name1 = base_match[1].str();
                            std::string table_name2 = base_match[2].str();
                            if ((table_name1 == tables[0] && table_name2 == tables[2]) || (table_name1 == tables[2] && table_name2 == tables[0]))
                            {
                                cur_13_can_join = true;
                                break;
                            }
                        }
                    }
                    if (cur_13_can_join)
                    {
                        for (auto cond : and_conds)
                        {
                            if (std::regex_match(cond, base_match, re_join))
                            {
                                std::string table_name1 = base_match[1].str();
                                std::string table_name2 = base_match[2].str();
                                if ((table_name1 == tables[1] && table_name2 == tables[3]) || (table_name1 == tables[3] && table_name2 == tables[1]))
                                {
                                    auto tmp = tables[1];
                                    tables[1] = tables[2];
                                    tables[2] = tmp;
                                    actually_13_can_join = true;
                                }
                            }
                        }
                    }
                    if (!actually_13_can_join)
                    {
                        auto tmp = tables[1];
                        tables[1] = tables[3];
                        tables[3] = tmp;
                    }
                }

                for (int i = 0; i < tables.size(); i++)
                {
                    tablename_map[tables[i]] = "tmp_table" + std::to_string(i + 1);
                }
                // Node *leaf1 = new Node(0, table2conds[tables[0]], "", "", NODE_LEAF, false);
                // std::map<std::string, std::string> leaf1_map;
                // leaf1_map[tables[0]] = "127.0.0.1" + PORT;
                // leaf1->set_children(leaf1_map);
                std::string frag_table = read_kv_from_etcd(tables[0]);
                TableMeta tablemeta1(nlohmann::json::parse(frag_table));
                std::vector<FragmentMeta> fragments = tablemeta1.get_fragments();

                bool is_horizontal = false;
                if(fragments.size())
                {
                    auto fragment = fragments[0];
                    if (fragment.frag_is_horizontal())
                    {
                        is_horizontal = true;
                    }
                }
                std::string sp = table2conds[tables[0]];

                if(is_horizontal)
                {
                    Node *child = new Node(0, "", "", "", NODE_NOT_LEAF, true);
                    std::map<std::string,std::string> child_map;
                    int idx = 1;
                    for (auto fragment : fragments)
                    {
                        std::string conds = fragment.get_frag_predicate();
                        if(conflict(sp,conds)){
                            continue;
                        }
                        Node *frag = new Node(0, sp, "", "", NODE_LEAF, false);
                        std::map<std::string, std::string> frag_map;
                        frag_map[fragment.get_frag_table_name()]="127.0.0.1" + PORT;
                        frag->set_children(frag_map);
                        std::string cur_node_name = "tmp_table1_"+std::to_string(idx);
                        idx++;
                        child_map[cur_node_name]=fragment.get_frag_site_address();
                        write_to_etcd(cur_node_name, frag->convert_to_json());
                    }
                    child->set_children(child_map);
                    write_to_etcd("tmp_table1", child->convert_to_json());
                }
                else
                {
                    int vertical_counts = fragments.size();
                    if(vertical_counts == 2)
                    {
                        std::string jp = "";
                        std::string frag_col = fragments[0].get_frag_field().get_field_name();
                        jp = "tmp_table1_1."+frag_col+ "=" + "tmp_table1_2."+frag_col;
                        Node *child = new Node(0, "", jp, "", NODE_NOT_LEAF, false);
                        std::map<std::string, std::string> child_map;
                        auto site1 = fragments[0].get_frag_site_address();
                        auto site2 = fragments[1].get_frag_site_address();
                        child_map["tmp_table1_1"] = site1;
                        child_map["tmp_table1_2"] = site2;
                        child->set_children(child_map);
                        write_to_etcd("tmp_table1", child->convert_to_json());

                        std::string v1_ss = "";
                        std::string v2_ss = "";

                        for(auto it:col2conds)
                        {
                            auto col_name = it.first;
                            trim(col_name);
                            auto fields1 = fragments[0].get_fields();
                            auto fields2 = fragments[1].get_fields();
                            std::vector<std::string> fields_name1;
                            std::vector<std::string> fields_name2;

                            for(auto field : fields1) {
                                fields_name1.push_back(field.get_field_name());
                            }
                            for(auto field : fields2) {
                                fields_name2.push_back(field.get_field_name());
                            }
                            auto it1 = std::find(fields_name1.begin(), fields_name1.end(),col_name);
                            if(it1 != fields_name1.end()){
                                v1_ss += it.second + " and ";
                            }
                            auto it2 = std::find(fields_name2.begin(), fields_name2.end(), col_name);
                            if(it2 != fields_name2.end()){
                                v2_ss += it.second + " and ";
                            }
                                    
                        }
                        if(v1_ss.length() >=5){
                            v1_ss.erase(v1_ss.length()-5,5);
                        }
                        if(v2_ss.length() >=5){
                            v2_ss.erase(v2_ss.length()-5,5);
                        }
                        Node * child_tmp1 = new Node(0, v1_ss, "", "", NODE_LEAF, false);
                        Node * child_tmp2 = new Node(0, v2_ss, "", "", NODE_LEAF, false);
                        std::map<std::string,std::string> tmp1_map;
                        std::map<std::string,std::string> tmp2_map;
                        tmp1_map[fragments[0].get_frag_table_name()] = "127.0.0.1" + PORT;
                        tmp2_map[fragments[1].get_frag_table_name()] = "127.0.0.1" + PORT;
                        child_tmp1->set_children(tmp1_map);
                        child_tmp2->set_children(tmp2_map);
                        write_to_etcd("tmp_table1_1", child_tmp1->convert_to_json());
                        write_to_etcd("tmp_table1_2", child_tmp2->convert_to_json());
                    }
                }


                // Node *leaf2 = new Node(0, table2conds[tables[1]], "", "", NODE_LEAF, false);
                // std::map<std::string, std::string> leaf2_map;
                // leaf2_map[tables[1]] = "127.0.0.1" + PORT;
                // leaf2->set_children(leaf2_map);
                frag_table = read_kv_from_etcd(tables[1]);
                TableMeta tablemeta2(nlohmann::json::parse(frag_table));
                fragments = tablemeta2.get_fragments();

                is_horizontal = false;
                if(fragments.size())
                {
                    auto fragment = fragments[0];
                    if (fragment.frag_is_horizontal())
                    {
                        is_horizontal = true;
                    }
                }
                sp = table2conds[tables[1]];

                if(is_horizontal)
                {
                    Node *child = new Node(0, "", "", "", NODE_NOT_LEAF, true);
                    std::map<std::string,std::string> child_map;
                    int idx = 1;
                    for (auto fragment : fragments)
                    {
                        std::string conds = fragment.get_frag_predicate();
                        if(conflict(sp,conds)){
                            continue;
                        }
                        Node *frag = new Node(0, sp, "", "", NODE_LEAF, false);
                        std::map<std::string, std::string> frag_map;
                        frag_map[fragment.get_frag_table_name()]="127.0.0.1" + PORT;
                        frag->set_children(frag_map);
                        std::string cur_node_name = "tmp_table2_"+std::to_string(idx);
                        idx++;
                        child_map[cur_node_name]=fragment.get_frag_site_address();
                        write_to_etcd(cur_node_name, frag->convert_to_json());
                    }
                    child->set_children(child_map);
                    write_to_etcd("tmp_table2", child->convert_to_json());
                }
                else
                {
                    int vertical_counts = fragments.size();
                    if(vertical_counts == 2)
                    {
                        std::string jp = "";
                        std::string frag_col = fragments[0].get_frag_field().get_field_name();
                        jp = "tmp_table2_1."+frag_col+ "=" + "tmp_table2_2."+frag_col;
                        Node *child = new Node(0, "", jp, "", NODE_NOT_LEAF, false);
                        std::map<std::string, std::string> child_map;
                        auto site1 = fragments[0].get_frag_site_address();
                        auto site2 = fragments[1].get_frag_site_address();
                        child_map["tmp_table2_1"] = site1;
                        child_map["tmp_table2_2"] = site2;
                        child->set_children(child_map);
                        write_to_etcd("tmp_table2", child->convert_to_json());

                        std::string v1_ss = "";
                        std::string v2_ss = "";

                        for(auto it:col2conds)
                        {
                            auto col_name = it.first;
                            trim(col_name);
                            auto fields1 = fragments[0].get_fields();
                            auto fields2 = fragments[1].get_fields();
                            std::vector<std::string> fields_name1;
                            std::vector<std::string> fields_name2;

                            for(auto field : fields1) {
                                fields_name1.push_back(field.get_field_name());
                            }
                            for(auto field : fields2) {
                                fields_name2.push_back(field.get_field_name());
                            }
                            auto it1 = std::find(fields_name1.begin(), fields_name1.end(),col_name);
                            if(it1 != fields_name1.end()){
                                v1_ss += it.second + " and ";
                            }
                            auto it2 = std::find(fields_name2.begin(), fields_name2.end(), col_name);
                            if(it2 != fields_name2.end()){
                                v2_ss += it.second + " and ";
                            }
                                    
                        }
                        if(v1_ss.length() >=5){
                            v1_ss.erase(v1_ss.length()-5,5);
                        }
                        if(v2_ss.length() >=5){
                            v2_ss.erase(v2_ss.length()-5,5);
                        }
                        Node * child_tmp1 = new Node(0, v1_ss, "", "", NODE_LEAF, false);
                        Node * child_tmp2 = new Node(0, v2_ss, "", "", NODE_LEAF, false);
                        std::map<std::string,std::string> tmp1_map;
                        std::map<std::string,std::string> tmp2_map;
                        tmp1_map[fragments[0].get_frag_table_name()] = "127.0.0.1" + PORT;
                        tmp2_map[fragments[1].get_frag_table_name()] = "127.0.0.1" + PORT;
                        child_tmp1->set_children(tmp1_map);
                        child_tmp2->set_children(tmp2_map);
                        write_to_etcd("tmp_table2_1", child_tmp1->convert_to_json());
                        write_to_etcd("tmp_table2_2", child_tmp2->convert_to_json());
                    }
                }


                // Node *leaf3 = new Node(0, table2conds[tables[2]], "", "", NODE_LEAF, false);
                // std::map<std::string, std::string> leaf3_map;
                // leaf3_map[tables[2]] = "127.0.0.1" + PORT;
                // leaf3->set_children(leaf3_map);
                frag_table = read_kv_from_etcd(tables[2]);
                TableMeta tablemeta3(nlohmann::json::parse(frag_table));
                fragments = tablemeta3.get_fragments();

                is_horizontal = false;
                if(fragments.size())
                {
                    auto fragment = fragments[0];
                    if (fragment.frag_is_horizontal())
                    {
                        is_horizontal = true;
                    }
                }
                sp = table2conds[tables[2]];

                if(is_horizontal)
                {
                    Node *child = new Node(0, "", "", "", NODE_NOT_LEAF, true);
                    std::map<std::string,std::string> child_map;
                    int idx = 1;
                    for (auto fragment : fragments)
                    {
                        std::string conds = fragment.get_frag_predicate();
                        if(conflict(sp,conds)){
                            continue;
                        }
                        Node *frag = new Node(0, sp, "", "", NODE_LEAF, false);
                        std::map<std::string, std::string> frag_map;
                        frag_map[fragment.get_frag_table_name()]="127.0.0.1" + PORT;
                        frag->set_children(frag_map);
                        std::string cur_node_name = "tmp_table3_"+std::to_string(idx);
                        idx++;
                        child_map[cur_node_name]=fragment.get_frag_site_address();
                        write_to_etcd(cur_node_name, frag->convert_to_json());
                    }
                    child->set_children(child_map);
                    write_to_etcd("tmp_table3", child->convert_to_json());
                }
                else
                {
                    int vertical_counts = fragments.size();
                    if(vertical_counts == 2)
                    {
                        std::string jp = "";
                        std::string frag_col = fragments[0].get_frag_field().get_field_name();
                        jp = "tmp_table3_1."+frag_col+ "=" + "tmp_table3_2."+frag_col;
                        Node *child = new Node(0, "", jp, "", NODE_NOT_LEAF, false);
                        std::map<std::string, std::string> child_map;
                        auto site1 = fragments[0].get_frag_site_address();
                        auto site2 = fragments[1].get_frag_site_address();
                        child_map["tmp_table3_1"] = site1;
                        child_map["tmp_table3_2"] = site2;
                        child->set_children(child_map);
                        write_to_etcd("tmp_table3", child->convert_to_json());

                        std::string v1_ss = "";
                        std::string v2_ss = "";

                        for(auto it:col2conds)
                        {
                            auto col_name = it.first;
                            trim(col_name);
                            auto fields1 = fragments[0].get_fields();
                            auto fields2 = fragments[1].get_fields();
                            std::vector<std::string> fields_name1;
                            std::vector<std::string> fields_name2;

                            for(auto field : fields1) {
                                fields_name1.push_back(field.get_field_name());
                            }
                            for(auto field : fields2) {
                                fields_name2.push_back(field.get_field_name());
                            }
                            auto it1 = std::find(fields_name1.begin(), fields_name1.end(),col_name);
                            if(it1 != fields_name1.end()){
                                v1_ss += it.second + " and ";
                            }
                            auto it2 = std::find(fields_name2.begin(), fields_name2.end(), col_name);
                            if(it2 != fields_name2.end()){
                                v2_ss += it.second + " and ";
                            }
                                    
                        }
                        if(v1_ss.length() >=5){
                            v1_ss.erase(v1_ss.length()-5,5);
                        }
                        if(v2_ss.length() >=5){
                            v2_ss.erase(v2_ss.length()-5,5);
                        }
                        Node * child_tmp1 = new Node(0, v1_ss, "", "", NODE_LEAF, false);
                        Node * child_tmp2 = new Node(0, v2_ss, "", "", NODE_LEAF, false);
                        std::map<std::string,std::string> tmp1_map;
                        std::map<std::string,std::string> tmp2_map;
                        tmp1_map[fragments[0].get_frag_table_name()] = "127.0.0.1" + PORT;
                        tmp2_map[fragments[1].get_frag_table_name()] = "127.0.0.1" + PORT;
                        child_tmp1->set_children(tmp1_map);
                        child_tmp2->set_children(tmp2_map);
                        write_to_etcd("tmp_table3_1", child_tmp1->convert_to_json());
                        write_to_etcd("tmp_table3_2", child_tmp2->convert_to_json());
                    }
                }


                // Node *leaf4 = new Node(0, table2conds[tables[3]], "", "", NODE_LEAF, false);
                // std::map<std::string, std::string> leaf4_map;
                // leaf4_map[tables[3]] = "127.0.0.1" + PORT;
                // leaf4->set_children(leaf4_map);
                frag_table = read_kv_from_etcd(tables[3]);
                TableMeta tablemeta4(nlohmann::json::parse(frag_table));
                fragments = tablemeta4.get_fragments();

                is_horizontal = false;
                if(fragments.size())
                {
                    auto fragment = fragments[0];
                    if (fragment.frag_is_horizontal())
                    {
                        is_horizontal = true;
                    }
                }
                sp = table2conds[tables[3]];

                if(is_horizontal)
                {
                    Node *child = new Node(0, "", "", "", NODE_NOT_LEAF, true);
                    std::map<std::string,std::string> child_map;
                    int idx = 1;
                    for (auto fragment : fragments)
                    {
                        std::string conds = fragment.get_frag_predicate();
                        if(conflict(sp,conds)){
                            continue;
                        }
                        Node *frag = new Node(0, sp, "", "", NODE_LEAF, false);
                        std::map<std::string, std::string> frag_map;
                        frag_map[fragment.get_frag_table_name()]="127.0.0.1" + PORT;
                        frag->set_children(frag_map);
                        std::string cur_node_name = "tmp_table4_"+std::to_string(idx);
                        idx++;
                        child_map[cur_node_name]=fragment.get_frag_site_address();
                        write_to_etcd(cur_node_name, frag->convert_to_json());
                    }
                    child->set_children(child_map);
                    write_to_etcd("tmp_table4", child->convert_to_json());
                }
                else
                {
                    int vertical_counts = fragments.size();
                    if(vertical_counts == 2)
                    {
                        std::string jp = "";
                        std::string frag_col = fragments[0].get_frag_field().get_field_name();
                        jp = "tmp_table4_1."+frag_col+ "=" + "tmp_table4_2."+frag_col;
                        Node *child = new Node(0, "", jp, "", NODE_NOT_LEAF, false);
                        std::map<std::string, std::string> child_map;
                        auto site1 = fragments[0].get_frag_site_address();
                        auto site2 = fragments[1].get_frag_site_address();
                        child_map["tmp_table4_1"] = site1;
                        child_map["tmp_table4_2"] = site2;
                        child->set_children(child_map);
                        write_to_etcd("tmp_table4", child->convert_to_json());

                        std::string v1_ss = "";
                        std::string v2_ss = "";

                        for(auto it:col2conds)
                        {
                            auto col_name = it.first;
                            trim(col_name);
                            auto fields1 = fragments[0].get_fields();
                            auto fields2 = fragments[1].get_fields();
                            std::vector<std::string> fields_name1;
                            std::vector<std::string> fields_name2;

                            for(auto field : fields1) {
                                fields_name1.push_back(field.get_field_name());
                            }
                            for(auto field : fields2) {
                                fields_name2.push_back(field.get_field_name());
                            }
                            auto it1 = std::find(fields_name1.begin(), fields_name1.end(),col_name);
                            if(it1 != fields_name1.end()){
                                v1_ss += it.second + " and ";
                            }
                            auto it2 = std::find(fields_name2.begin(), fields_name2.end(), col_name);
                            if(it2 != fields_name2.end()){
                                v2_ss += it.second + " and ";
                            }
                                    
                        }
                        if(v1_ss.length() >=5){
                            v1_ss.erase(v1_ss.length()-5,5);
                        }
                        if(v2_ss.length() >=5){
                            v2_ss.erase(v2_ss.length()-5,5);
                        }
                        Node * child_tmp1 = new Node(0, v1_ss, "", "", NODE_LEAF, false);
                        Node * child_tmp2 = new Node(0, v2_ss, "", "", NODE_LEAF, false);
                        std::map<std::string,std::string> tmp1_map;
                        std::map<std::string,std::string> tmp2_map;
                        tmp1_map[fragments[0].get_frag_table_name()] = "127.0.0.1" + PORT;
                        tmp2_map[fragments[1].get_frag_table_name()] = "127.0.0.1" + PORT;
                        child_tmp1->set_children(tmp1_map);
                        child_tmp2->set_children(tmp2_map);
                        write_to_etcd("tmp_table4_1", child_tmp1->convert_to_json());
                        write_to_etcd("tmp_table4_2", child_tmp2->convert_to_json());
                    }
                }


                Node *tmp_table5;
                bool leaf12_eq_join = false;
                std::string tmp_table5_pp = "";
                // for (auto s : columns)
                // {
                //     std::string cur_tablename = s.substr(0, s.find("."));
                //     if (cur_tablename == tables[0] || cur_tablename == tables[1])
                //     {
                //         s.replace(s.find(cur_tablename), cur_tablename.length(), tablename_map[cur_tablename]);
                //         s.replace(s.find("."), 1, "." + cur_tablename + "_");
                //         tmp_table5_pp += s + ", ";
                //     }
                // }
                // tmp_table5_pp.erase(tmp_table5_pp.find_last_of(","));

                for (auto cond : and_conds)
                {
                    if (std::regex_match(cond, base_match, re_join))
                    {
                        std::string table_name1 = base_match[1].str();
                        std::string table_name2 = base_match[2].str();

                        if ((table_name1 == tables[0] && table_name2 == tables[1]) || (table_name1 == tables[1] && table_name2 == tables[0]))
                        {
                            leaf12_eq_join = true;
                            cond.replace(cond.find(table_name1), table_name1.length(), tablename_map[table_name1]);
                            cond.replace(cond.rfind(table_name2), table_name2.length(), tablename_map[table_name2]);
                            cond.replace(cond.find_first_of("."), 1, "." + table_name1 + "_");
                            cond.replace(cond.find_last_of("."), 1, "." + table_name2 + "_");
                            tmp_table5 = new Node(0, "", cond, tmp_table5_pp, NODE_NOT_LEAF, false);
                            break;
                        }
                    }
                }
                if (!leaf12_eq_join)
                {
                    tmp_table5 = new Node(0, "", "", tmp_table5_pp, NODE_NOT_LEAF, false);
                }
                std::map<std::string, std::string> tmp_table5_map;
                tmp_table5_map["tmp_table1"] = "127.0.0.1" + PORT; //leaf1
                tmp_table5_map["tmp_table2"] = "127.0.0.1" + PORT; //leaf2
                tmp_table5->set_children(tmp_table5_map);

                Node *tmp_table6;
                bool leaf34_eq_join = false;

                std::string tmp_table6_pp = "";
                // for (auto s : columns)
                // {
                //     std::string cur_tablename = s.substr(0, s.find("."));
                //     if (cur_tablename == tables[2] || cur_tablename == tables[3])
                //     {
                //         s.replace(s.find(cur_tablename), cur_tablename.length(), tablename_map[cur_tablename]);
                //         s.replace(s.find("."), 1, "." + cur_tablename + "_");
                //         tmp_table6_pp += s + ", ";
                //     }
                // }
                // tmp_table6_pp.erase(tmp_table6_pp.find_last_of(","));

                for (auto cond : and_conds)
                {
                    if (std::regex_match(cond, base_match, re_join))
                    {
                        std::string table_name1 = base_match[1].str();
                        std::string table_name2 = base_match[2].str();

                        if ((table_name1 == tables[2] && table_name2 == tables[3]) || (table_name1 == tables[3] && table_name2 == tables[2]))
                        {
                            leaf34_eq_join = true;
                            cond.replace(cond.find(table_name1), table_name1.length(), tablename_map[table_name1]);
                            cond.replace(cond.rfind(table_name2), table_name2.length(), tablename_map[table_name2]);
                            cond.replace(cond.find_first_of("."), 1, "." + table_name1 + "_");
                            cond.replace(cond.find_last_of("."), 1, "." + table_name2 + "_");
                            tmp_table6 = new Node(0, "", cond, tmp_table6_pp, NODE_NOT_LEAF, false);
                            break;
                        }
                    }
                }
                if (!leaf34_eq_join)
                {
                    tmp_table6 = new Node(0, "", "", tmp_table6_pp, NODE_NOT_LEAF, false);
                }
                std::map<std::string, std::string> tmp_table6_map;
                tmp_table6_map["tmp_table3"] = "127.0.0.1" + PORT; //leaf3
                tmp_table6_map["tmp_table4"] = "127.0.0.1" + PORT; //leaf4
                tmp_table6->set_children(tmp_table6_map);

                Node *node;
                bool tmp56_eq_join = false;
                std::string node_pp = "";
                for (auto s : columns)
                {
                    std::string cur_tablename = s.substr(0, s.find("."));
                    if (cur_tablename == tables[0] || cur_tablename == tables[1])
                    {
                        s.replace(s.find(cur_tablename), cur_tablename.length(), "tmp_table5");
                        s.replace(s.find("."), 1, "." + cur_tablename + "_");
                        node_pp += s + ", ";
                    }
                    else if (cur_tablename == tables[2] || cur_tablename == tables[3])
                    {
                        s.replace(s.find(cur_tablename), cur_tablename.length(), "tmp_table6");
                        s.replace(s.find("."), 1, "." + cur_tablename + "_");
                        node_pp += s + ", ";
                    }
                }
                node_pp.erase(node_pp.find_last_of(","));
                std::string node_jp = "";
                for (auto cond : and_conds)
                {
                    if (std::regex_match(cond, base_match, re_join))
                    {
                        std::string table_name1 = base_match[1].str();
                        std::string table_name2 = base_match[2].str();

                        if (((table_name1 == tables[0] || table_name1 == tables[1]) && (table_name2 == tables[2] || table_name2 == tables[3])) ||
                            ((table_name1 == tables[2] || table_name1 == tables[3]) && (table_name2 == tables[0] || table_name2 == tables[1])))
                        {
                            tmp56_eq_join = true;
                            if ((table_name1 == tables[0] || table_name1 == tables[1]))
                            {
                                cond.replace(cond.find(table_name1), table_name1.length(), "tmp_table5");
                                cond.replace(cond.find(table_name2), table_name2.length(), "tmp_table6");
                            }
                            else
                            {
                                cond.replace(cond.find(table_name1), table_name1.length(), "tmp_table6");
                                cond.replace(cond.find(table_name2), table_name2.length(), "tmp_table5");
                            }
                            cond.replace(cond.find_first_of("."), 1, "." + table_name1 + "_");
                            cond.replace(cond.find_last_of("."), 1, "." + table_name2 + "_");
                            node_jp += cond + " and ";
                        }
                    }
                }
                if (!tmp56_eq_join)
                {
                    node = new Node(0, "", "", node_pp, NODE_NOT_LEAF, false);
                }
                else
                {
                    node_jp.erase(node_jp.rfind("and"));
                    node = new Node(0, "", node_jp, node_pp, NODE_NOT_LEAF, false);
                }
                std::map<std::string, std::string> node_map;
                node_map["tmp_table5"] = "127.0.0.1" + PORT;
                node_map["tmp_table6"] = "127.0.0.1" + PORT;
                node->set_children(node_map);

                write_to_etcd("root_node", node->convert_to_json());
                // write_to_etcd("tmp_table1", leaf1->convert_to_json());
                // write_to_etcd("tmp_table2", leaf2->convert_to_json());
                // write_to_etcd("tmp_table3", leaf3->convert_to_json());
                // write_to_etcd("tmp_table4", leaf4->convert_to_json());
                write_to_etcd("tmp_table5", tmp_table5->convert_to_json());
                write_to_etcd("tmp_table6", tmp_table6->convert_to_json());
            }
        }
        return "root_node";
    }
    else if (std::regex_match(input, base_match, re_select))
    {
        std::vector<std::string> columns;
        std::vector<std::string> tables;
        std::string column_str;
        std::string table_str;
        if (base_match.size() >= 3)
        {
            column_str = base_match[1].str();
            table_str = base_match[2].str();
            str_split(column_str, columns);
            str_split(table_str, tables);
        }
        trim(column_str);
        trim(table_str);

        if (tables.size() == 1)
        {
            //one table no join
            std::string tablename = tables[0];
            std::string project_predicate = columns[0];
            if (project_predicate != "*")
            {
                for (auto &s : columns)
                {
                    s.replace(s.find(tablename), tablename.length(), "tmp_table");
                    s.replace(s.find("."), 1, "." + tablename + "_");
                }
                project_predicate = columns[0];
                for (int i = 1; i < columns.size(); i++)
                {
                    project_predicate += ", " + columns[i];
                }
            }
            Node *node = new Node(0, "", "", project_predicate, NODE_NOT_LEAF, false);
            std::map<std::string, std::string> node_map;
            node_map["tmp_table"] = "127.0.0.1" + PORT;
            node->set_children(node_map);

            Node *child = new Node(0, "", "", "", NODE_LEAF, false);
            std::map<std::string, std::string> child_map;
            child_map[tablename] = "127.0.0.1" + PORT;
            child->set_children(child_map);

            write_to_etcd("tmp_table", child->convert_to_json());
            write_to_etcd("root_node", node->convert_to_json());
            return "root_node";
        }
    }
}

int execute_non_query_sql(const std::string &host, const std::string &sql){
    // A Channel represents a communication line to a Server. Notice that
    // Channel is thread-safe and can be shared by all threads in your program.
    brpc::Channel channel;

    // Initialize the channel, NULL means using default options.
    brpc::ChannelOptions options;
    options.protocol = FLAGS_protocol_service;
    options.connection_type = FLAGS_connection_type_service;
    options.timeout_ms = FLAGS_timeout_ms_service/*milliseconds*/;
    options.max_retry = FLAGS_max_retry_service;
    if (channel.Init(host.c_str(), FLAGS_load_balancer_service.c_str(), &options) != 0) {
        LOG(ERROR) << "Fail to initialize channel";
        return -1;
    }

    // Normally, you should not call a Channel directly, but instead construct
    // a stub Service wrapping it. stub can be shared by all threads as well.
    sql_service_Stub stub(&channel);

    {
        brpc::Controller* cntl = new brpc::Controller;
        ExecuteNonQuerySQLResponse* response = new ExecuteNonQuerySQLResponse;

        ExecuteNonQuerySQLRequest request;
        request.set_sql_query(sql);

        LOG(INFO) << "Send query to " << host << " query:";
        LOG(INFO) << sql;

        stub.ExecuteNonQuerySQL(cntl, &request, response, nullptr);
        if (cntl->Failed()) {
            LOG(WARNING) << "Some site was down, " << cntl->ErrorText();
            return -1;
        }
        LOG(INFO) << "Received response from " << cntl->remote_side() << " sql:";// << "\nLatency(us): " << cntl->latency_us();
        LOG(INFO) << sql;
        LOG(INFO) << "Result: " << response->result();
    }

    return 0;
}

void show_sites() {
    std::string site_list = read_kv_from_etcd("sites");
    nlohmann::json sites;
    if(site_list == "") {
        LOG(ERROR) << "There are no tables in etcd.";
        return;
    } else{
        sites = nlohmann::json::parse(site_list);
    }
    int site_num = sites["value"]["num"].get<int>();
    for(int i = 0; i < site_num; i++){
        std::string index = std::to_string(i);
        std::string site_name = sites["value"][index];
        std::cout << SEPERATE_LINE << std::endl;
        std::cout << SEPERATE_SIGN << site_name << std::endl;
        std::string site_string = read_kv_from_etcd(site_name);
        if(site_string  == ""){
            LOG(ERROR) << "No site named " << site_name << ", metadata error!";
            return;
        }
        std::cout << SEPERATE_SIGN << site_string << std::endl;
    }
    std::cout << SEPERATE_LINE << std::endl;
    std::cout << "Total site num: " << site_num << std::endl;
}

void create_site(const std::string &site_name, const std::string &address) {
    
    // create site struct and write to etcd
    Site site(address, site_name);
    nlohmann::json site_json = site.convert_to_json();
    if(write_to_etcd(site_name, site_json) != 0){
        LOG(ERROR) << "Write site to etcd failed!";
        return;
    }

    // get the metadata list of sites, update it and write back
    std::string sites_list = read_kv_from_etcd("sites");
    nlohmann::json sites;
    if(sites_list == "") {
        sites["value"]["num"] = 0; 
    } else {
        sites = nlohmann::json::parse(sites_list);
    }
    
    int site_num = sites["value"]["num"].get<int>();
    std::string index = std::to_string(site_num);
    sites["value"][index] = site_name;
    sites["value"]["num"] = site_num+1;
    if(write_to_etcd("sites", sites) != 0) {
        LOG(ERROR) << "Write site metadata to etcd failed!";
        return;
    }

    LOG(INFO) << "site " << site_name << " created, address is " << address; 
    return;
}

void show_tables() {
    std::string table_list = read_kv_from_etcd("tables");
    nlohmann::json tables;
    if(table_list == "") {
        LOG(ERROR) << "There are no tables in etcd.";
        return;
    } else{
        tables = nlohmann::json::parse(table_list);
    }
    //LOG(INFO) << "Tables " << tables;
    int table_num = tables["value"]["num"];
    for(int i = 0; i < table_num; i++){
        std::string index = std::to_string(i);
        std::string table_name = tables["value"][index];
        std::cout << SEPERATE_LINE << std::endl;
        std::cout << SEPERATE_SIGN << table_name << std::endl;
        std::string table_string = read_kv_from_etcd(table_name);
        if(table_string  == ""){
            LOG(ERROR) << "No table named " << table_name << ", metadata error!";
            return;
        }
        std::cout << SEPERATE_SIGN << table_string << std::endl;
    }
    std::cout << SEPERATE_LINE << std::endl;
    std::cout << "Total table num: " << table_num << std::endl;
}

void create_table(TableMeta table) {
    
    // create site struct and write to etcd
    nlohmann::json table_json = table.convert_to_json();
    if(write_to_etcd(table.get_table_name(), table_json) != 0){
        LOG(ERROR) << "Write table to etcd failed!";
        return;
    }

    // get the metadata list of sites, update it and write back
    std::string table_list = read_kv_from_etcd("tables");
    nlohmann::json tables;
    if(table_list == "") {
        tables["value"]["num"] = 0;
    } else{
        tables = nlohmann::json::parse(table_list);
    }
    int table_num = tables["value"]["num"].get<int>();
    std::string index = std::to_string(table_num);
    tables["value"][index] = table.get_table_name();
    tables["value"]["num"] = table_num+1;
    if(write_to_etcd("tables", tables) != 0) {
        LOG(ERROR) << "Write table metadata to etcd failed!";
        return;
    }

    LOG(INFO) << "table " << table.get_table_name() << " created" ; 
    // LOG(INFO) << table_json.dump();
    return;
}

void show_fragments_of_table(const std::string &table_name) {
    std::cout << "Fragments of table " << table_name << std::endl;
    if(table_name == "") {
        LOG(ERROR) << "Enter the table name";
        return;
    }
    std::string table_string = read_kv_from_etcd(table_name);
    if(table_string == "") {
        LOG(ERROR) << "No table named: " << table_name;
        return;
    }
    nlohmann::json table_json = nlohmann::json::parse(table_string);
    
    // std::cout << SEPERATE_SIGN << table_json.dump() << std::endl;


    int frag_num = table_json["value"]["table_fragments"]["num"].get<int>();
    for(int i = 0; i < frag_num; i++){
        std::string index = std::to_string(i);
        nlohmann::json fragment = table_json["value"]["table_fragments"][index];
        // std::cout << SEPERATE_LINE << std::endl;
        std::cout << SEPERATE_SIGN << fragment["value"]["frag_name"] << std::endl;
        // std::cout << SEPERATE_SIGN << fragment.dump() << std::endl;
    }
    
    std::cout << "Total fragment num: " << frag_num << std::endl;
    std::cout << SEPERATE_LINE << std::endl;
}

void show_fragments_of_site(const std::string &site_name) {
    std::cout << "Fragments on site " << site_name << std::endl;
    
    if(site_name == "") {
        LOG(ERROR) << "Enter the site name";
        return;
    }
    std::string site_string = read_kv_from_etcd(site_name);
    if(site_string == "") {
        LOG(ERROR) << "No site named: " << site_name;
        return;
    }
    nlohmann::json site_json = nlohmann::json::parse(site_string);
    
    // std::cout << SEPERATE_SIGN << site_json.dump() << std::endl;

    int frag_num = site_json["value"]["site_fragments"]["num"].get<int>();
    for(int i = 0; i < frag_num; i++){
        std::string index = std::to_string(i);
        nlohmann::json fragment = site_json["value"]["site_fragments"][index];
        // std::cout << SEPERATE_LINE << std::endl;
        // std::cout << SEPERATE_SIGN << fragment.dump() << std::endl;
        std::cout << SEPERATE_SIGN << fragment["value"]["frag_name"] << std::endl;
    }
    
    std::cout << "Total fragment num: " << frag_num << std::endl;
    std::cout << SEPERATE_LINE << std::endl;
}

void create_fragment(FragmentMeta &frag) {
    nlohmann::json frag_json = frag.convert_to_json();

    // get the fragments list in table, update it and write back
    std::string frag_table = read_kv_from_etcd(frag.get_frag_table_name());
    nlohmann::json table_json;
    if(frag_table == "") {
        LOG(ERROR) << "Table " << frag.get_frag_table_name() << " not exist! check the table name";
        return;
    }
    table_json = nlohmann::json::parse(frag_table);
    // get the fragments list in site, update it and write back
    std::string frag_site = read_kv_from_etcd(frag.get_frag_site_name());
    nlohmann::json site_json;
    if(frag_site == "") {
        LOG(ERROR) << "Site " << frag.get_frag_site_name() << " not exist! check the site name";
        return;
    }
    site_json = nlohmann::json::parse(frag_site);

    
    int table_frag_num = table_json["value"]["table_fragments"]["num"].get<int>();
    std::string table_index = std::to_string(table_frag_num);
    table_json["value"]["table_fragments"][table_index] = frag_json;
    table_json["value"]["table_fragments"]["num"] = table_frag_num+1;
    

    int site_frag_num = site_json["value"]["site_fragments"]["num"].get<int>();
    std::string site_index = std::to_string(site_frag_num);
    site_json["value"]["site_fragments"][site_index] = frag_json;
    site_json["value"]["site_fragments"]["num"] = site_frag_num+1;
    
    frag_json["value"]["frag_site_address"] = site_json["value"]["site_address"];

    if(write_to_etcd(frag.get_frag_name(), frag_json) != 0){
        LOG(ERROR) << "Write frag to etcd failed!";
        return;
    }

    if(write_to_etcd(frag.get_frag_site_name(), site_json) != 0) {
        LOG(ERROR) << "Write table metadata to etcd failed!";
        return;
    }

    if(write_to_etcd(frag.get_frag_table_name(), table_json) != 0) {
        LOG(ERROR) << "Write table metadata to etcd failed!";
        return;
    }

    LOG(INFO) << "fragment " << frag.get_frag_name() << " created, fragment is ";
    LOG(INFO) << frag_json.dump(); 

    std::string host = site_json["value"]["site_address"].get<std::string>();
    execute_non_query_sql(host, frag.get_frag_create_table_sql());
    
    return;
}

void parse_non_query(std::string sql) {
    std::smatch base_match;
    if (std::regex_match(sql, base_match, re_define_site))
    {
        std::string site_name = base_match[1].str();
        std::string address = base_match[2].str();
        create_site(site_name, address);
    }
    else if (std::regex_match(sql, base_match, re_create_db))
    {
        std::string db_name = base_match[1].str();
        // create_db(db_name);
    }
    else if (std::regex_match(sql, base_match, re_fragment_h))
    {
        std::string frag_name = base_match[1].str();
        std::string table_name = base_match[2].str();
        std::string site_name = base_match[3].str();
        std::string field_name = base_match[4].str();
        std::string range = base_match[5].str();
    }
    else if (std::regex_match(sql, base_match, re_fragment_v))
    {
        std::string frag_name = base_match[1].str();
        std::string table_name = base_match[2].str();
        std::string site_name = base_match[3].str();
        std::string pk_name = base_match[4].str();
        std::string field_list = base_match[5].str();
    }
}

void handle_create_table(std::string sql){
    std::string newsql="create table ";
    std::smatch base_match;
    if (std::regex_match(sql, base_match, re_create_table)){
        std::string tablename = base_match[1].str();
        std::string info = base_match[2].str();
        trim(tablename);
        tablename = tablename.substr(1,tablename.length()-2);
        trim(info);
        std::vector<std::string> parts;
        str_split(info,parts);
        std::vector<std::string> cols,types;
        for(auto& part:parts){
            if(part[0]=='`'){
                //it's column
                std::string colname = part.substr(1,part.find_last_of("`")-1);
                cols.push_back(tablename+"_"+colname);
                std::vector<std::string> tmp;
                str_split(part,tmp," ");
                if(tmp[1]=="int"){
                    types.push_back("INT");
                }
                if(tmp[1].substr(0,7)=="varchar"){
                    types.push_back("VARCHAR");
                }             
            }
            part.replace(part.find_first_of("`"),1,"`"+tablename+"_");
        }
        newsql += tablename+" ( ";
        for(auto part:parts){
            newsql+=part+", ";
        }
        newsql.replace(newsql.find_last_of(","),2," );");
        std::vector<Field> fields;
        for(int i=0;i<cols.size();i++){
            Field field(types[i],cols[i]);
            fields.push_back(field);
        }
        TableMeta tmeta(tablename, 0, fields, 0, newsql);
        create_table(tmeta);
    }
}




std::vector<std::string> execute_query_sql(const std::string& host, const std::string& sql) {
    brpc::Channel channel;
    brpc::ChannelOptions options;
    options.protocol = FLAGS_protocol_service;
    options.connection_type = FLAGS_connection_type_service;
    options.timeout_ms = FLAGS_timeout_ms_service/*milliseconds*/;
    options.max_retry = FLAGS_max_retry_service;
    std::vector<std::string> v;

    if (channel.Init(host.c_str(), FLAGS_load_balancer_service.c_str(), &options) != 0){
        LOG(ERROR) << "Fail to initialize channel";
        return v;
    }

    brpc::Controller *cntl;
    ExecuteQuerySQLRequest *request;
    ExecuteQuerySQLResponse *response;
    request->set_sql(sql);
    sql_service_Stub stub(&channel);
    stub.ExecuteQuerySQL(cntl, request, response, nullptr);
    if (cntl->Failed()) {
        LOG(WARNING) << "Some site was down, " << cntl->ErrorText();
        return v;
    }
    v.push_back(response->table_fields());
    for (int i = 0; i < response->table_values_size(); ++i) {
        v.push_back(response->table_values(i));
    }

    return v;
}





void HandleRequestNodeResponse(
        brpc::Controller* cntl,
        NodeResponse* response) {
    // std::unique_ptr makes sure cntl/response will be deleted before returning.
    std::unique_ptr<brpc::Controller> cntl_guard(cntl);
    std::unique_ptr<NodeResponse> response_guard(response);

    if (cntl->Failed()) {
        LOG(WARNING) << "Some site was down, " << cntl->ErrorText();
        return;
    }
    std::cout << "Result meta: " << response->table_fields() << std::endl;
    std::cout << "Result: " << response->table_values().size() << std::endl;
    for (int i = 0; i < response->table_values_size(); ++i) {
        std::cout << response->table_values(i) << std::endl;
    }
}

void HandleImportDataResponse(brpc::Controller* cntl, ImportDataResponse* response){
    // std::unique_ptr makes sure cntl/response will be deleted before returning.
    std::unique_ptr<brpc::Controller> cntl_guard(cntl);
    std::unique_ptr<ImportDataResponse> response_guard(response);

    if (cntl->Failed()) {
        LOG(WARNING) << "Some site was down, " << cntl->ErrorText();
        return;
    }
    LOG(INFO) << "Result: " << response->result();
}


int import_data(std::string host, std::string table_name, std::string table_fields, std::vector<std::string> table_values){
    // A Channel represents a communication line to a Server. Notice that
    // Channel is thread-safe and can be shared by all threads in your program.
    brpc::Channel channel;

    // Initialize the channel, NULL means using default options.
    brpc::ChannelOptions options;
    options.protocol = FLAGS_protocol_service;
    options.connection_type = FLAGS_connection_type_service;
    options.timeout_ms = FLAGS_timeout_ms_service/*milliseconds*/;
    options.max_retry = FLAGS_max_retry_service;
    if (channel.Init(host.c_str(), FLAGS_load_balancer_service.c_str(), &options) != 0) {
        LOG(ERROR) << "Fail to initialize channel";
        return -1;
    }

    // Normally, you should not call a Channel directly, but instead construct
    // a stub Service wrapping it. stub can be shared by all threads as well.
    sql_service_Stub stub(&channel);

    {
        brpc::Controller* cntl = new brpc::Controller;
        ImportDataResponse* response = new ImportDataResponse;

        ImportDataRequest request;
        request.set_table_name(table_name);
        request.set_table_fields(table_fields);
        for(const std::string& table_value : table_values){
            request.add_table_values(table_value);
        }

        LOG(INFO) << "Send insert rpc to site " << host;

        stub.ImportData(cntl, &request, response, nullptr);
        if (cntl->Failed()) {
            LOG(WARNING) << "Some site was down, " << cntl->ErrorText();
            return -1;
        }
        LOG(INFO) << "Received response from " << cntl->remote_side(); // << "\nLatency(us): " << cntl->latency_us();
        LOG(INFO) << "Result: " << response->result();
    }

    return 0;
}

int create_table(std::string host, std::string table_name, std::string sql){
    // A Channel represents a communication line to a Server. Notice that
    // Channel is thread-safe and can be shared by all threads in your program.
    brpc::Channel channel;

    // Initialize the channel, NULL means using default options.
    brpc::ChannelOptions options;
    options.protocol = FLAGS_protocol_service;
    options.connection_type = FLAGS_connection_type_service;
    options.timeout_ms = FLAGS_timeout_ms_service/*milliseconds*/;
    options.max_retry = FLAGS_max_retry_service;
    if (channel.Init(host.c_str(), FLAGS_load_balancer_service.c_str(), &options) != 0) {
        LOG(ERROR) << "Fail to initialize channel";
        return -1;
    }

    // Normally, you should not call a Channel directly, but instead construct
    // a stub Service wrapping it. stub can be shared by all threads as well.
    sql_service_Stub stub(&channel);

    // async client
    {
        brpc::Controller* cntl = new brpc::Controller;
        ExecuteNonQuerySQLResponse* response = new ExecuteNonQuerySQLResponse;

        ExecuteNonQuerySQLRequest request;
        request.set_sql_query(sql);

//        google::protobuf::Closure* done = brpc::NewCallback(&HandleLoadTableResponse, cntl, response);

        stub.ExecuteNonQuerySQL(cntl, &request, response, nullptr);
        if (cntl->Failed()) {
            LOG(WARNING) << "Some site was down, " << cntl->ErrorText();
            return -1;
        }
        LOG(INFO) << "Received response from " << cntl->remote_side() << "\nLatency(us): " << cntl->latency_us();
        LOG(INFO) << "Result: " << response->result();
    }

    return 0;
}

int delete_table(std::string host, std::string table_name){
    // A Channel represents a communication line to a Server. Notice that
    // Channel is thread-safe and can be shared by all threads in your program.
    brpc::Channel channel;

    // Initialize the channel, NULL means using default options.
    brpc::ChannelOptions options;
    options.protocol = FLAGS_protocol_service;
    options.connection_type = FLAGS_connection_type_service;
    options.timeout_ms = FLAGS_timeout_ms_service/*milliseconds*/;
    options.max_retry = FLAGS_max_retry_service;
    if (channel.Init(host.c_str(), FLAGS_load_balancer_service.c_str(), &options) != 0) {
        LOG(ERROR) << "Fail to initialize channel";
        return -1;
    }

    // Normally, you should not call a Channel directly, but instead construct
    // a stub Service wrapping it. stub can be shared by all threads as well.
    sql_service_Stub stub(&channel);

    // async client
    {
        brpc::Controller* cntl = new brpc::Controller;
        DeleteTempTableResponse* response = new DeleteTempTableResponse;

        DeleteTempTableRequest request;
        request.set_temp_table_name(table_name);

//        google::protobuf::Closure* done = brpc::NewCallback(&HandleLoadTableResponse, cntl, response);

        stub.DeleteTable(cntl, &request, response, nullptr);
        if (cntl->Failed()) {
            LOG(WARNING) << "Some site was down, " << cntl->ErrorText();
            return -1;
        }
        LOG(INFO) << "Received response from " << cntl->remote_side() << "\nLatency(us): " << cntl->latency_us();
        LOG(INFO) << "Result: " << response->result();
    }

    return 0;
}

std::vector<std::string> request_node(std::string temp_table_name, std::string host){
    // A Channel represents a communication line to a Server. Notice that
    // Channel is thread-safe and can be shared by all threads in your program.
    brpc::Channel channel;

    // Initialize the channel, NULL means using default options.
    brpc::ChannelOptions options;
    options.protocol = FLAGS_protocol_service;
    options.connection_type = FLAGS_connection_type_service;
    options.timeout_ms = FLAGS_timeout_ms_service/*milliseconds*/;
    options.max_retry = FLAGS_max_retry_service;
    if (channel.Init(host.c_str(), FLAGS_load_balancer_service.c_str(), &options) != 0) {
        LOG(ERROR) << "Fail to initialize channel";
        std::vector<std::string> rs = {""};
        return rs;
    }

    // Normally, you should not call a Channel directly, but instead construct
    // a stub Service wrapping it. stub can be shared by all threads as well.
    sql_service_Stub stub(&channel);


    // async client
    
    brpc::Controller* cntl = new brpc::Controller;
    NodeResponse* response = new NodeResponse;

    NodeRequest request;
    request.set_temp_table_name(temp_table_name);

//        google::protobuf::Closure* done = brpc::NewCallback(
//                &HandleRequestTableResponse, cntl, response);

    stub.RequestNode(cntl, &request, response, NULL);

    if (cntl->Failed()) {
        LOG(WARNING) << "Some site was down, " << cntl->ErrorText();
        std::vector<std::string> rs = {""};
        return rs;
    }
    LOG(INFO) << "Received response from " << cntl->remote_side() << "\nLatency(us): " << cntl->latency_us();
    std::cout << "Result meta: " << response->table_fields() << std::endl;
    std::cout << "Result count: " << response->table_values().size() << std::endl;
    std::vector<std::string> result;
    for (int i = 0; i < response->table_values_size(); ++i) {
        result.push_back(response->table_values(i));
    }

    // delete root temp table
    cntl->Reset();
    DeleteTempTableResponse* delete_response = new DeleteTempTableResponse;
    DeleteTempTableRequest delete_request;
    delete_request.set_temp_table_name(temp_table_name);
    stub.DeleteTable(cntl, &delete_request, delete_response, NULL);
    std::cout << "Result count: " << response->table_values().size() << std::endl;
    

    return result;
}

int test_query_7(const std::string &host) {
    Node root_node(0, "", "temp_table_1.id = temp_table_2.customer_id", "temp_table_1.name, temp_table_1.rank, temp_table_2.quantity", NODE_NOT_LEAF, false);
    Node temp_table_1(0, "customer.rank=1", "", "", NODE_LEAF, false);
    Node temp_table_2(0, "", "", "", NODE_LEAF, false);
    root_node.add_children("temp_table_1", "127.0.0.1");
    root_node.add_children("temp_table_2", "127.0.0.1");
    temp_table_1.add_children("customer", "127.0.0.1");
    temp_table_2.add_children("orders", "127.0.0.1");
    nlohmann::json root_json = root_node.convert_to_json();
    nlohmann::json temp1_json = temp_table_1.convert_to_json();
    nlohmann::json temp2_json = temp_table_2.convert_to_json();
    write_to_etcd("root_table", root_json);
    write_to_etcd("temp_table_1", temp1_json);
    write_to_etcd("temp_table_2", temp2_json);

    request_node("root_table", host);
}

int test_query_7_with_2nodes(const std::string &host) {
    Node root_node(0, "", "temp_table_1.customer_id = temp_table_2.orders_customer_id", "temp_table_1.customer_name, temp_table_1.customer_rank, temp_table_2.orders_quantity", NODE_NOT_LEAF, false);
    Node temp_table_1(0, "customer.customer_rank=1", "", "", NODE_LEAF, false);
    Node temp_table_2(0, "", "", "", NODE_NOT_LEAF, true);
    Node temp_table_2_1(0, "", "", "", NODE_LEAF, false);
    Node temp_table_2_2(0, "", "", "", NODE_LEAF, false);

    root_node.add_children("temp_table_1", "127.0.0.1:7379");
    root_node.add_children("temp_table_2", "127.0.0.1:7379");
    temp_table_1.add_children("customer", "127.0.0.1:7379");
    temp_table_2.add_children("temp_table_2_1", "127.0.0.1:7379");
    temp_table_2.add_children("temp_table_2_2", "127.0.0.1:8000");
    temp_table_2_1.add_children("orders", "127.0.0.1:7379");
    temp_table_2_2.add_children("orders", "127.0.0.1:8000");
    
    nlohmann::json root_json = root_node.convert_to_json();
    nlohmann::json temp1_json = temp_table_1.convert_to_json();
    nlohmann::json temp2_json = temp_table_2.convert_to_json();
    nlohmann::json temp2_1_json = temp_table_2_1.convert_to_json();
    nlohmann::json temp2_2_json = temp_table_2_2.convert_to_json();
    write_to_etcd("root_table", root_json);
    write_to_etcd("temp_table_1", temp1_json);
    write_to_etcd("temp_table_2", temp2_json);
    write_to_etcd("temp_table_2_1", temp2_1_json);
    write_to_etcd("temp_table_2_2", temp2_2_json);

    request_node("root_table", host);
}

std::string create_insert_value_for_horizontal(std::vector<std::string> fields, std::vector<std::string> types){
    auto type_it = types.begin();
    std::string value;
    for(auto field : fields) {
        if(*type_it == "INT") {
            value = value + " " + field + ",";
        }
        else {
            value = value + " " + "\'" + field + "\'" + ",";
        }
        type_it++;
    }
    value = value.substr(0, value.length() - 1);
    
    return value;
}

std::map<std::string, std::string> create_insert_value_for_vertical(TableMeta table, 
                                                                    std::map<std::string, std::string> fields) {
    std::vector<FragmentMeta> frags = table.get_fragments();
    std::map<std::string, std::string> values;
    for(auto frag_it = frags.begin(); frag_it != frags.end(); frag_it++) {
        std::string value;
        std::string site_address = frag_it->get_frag_site_address();
        std::vector<Field> frag_fields = frag_it->get_fields();
        for(Field frag_field : frag_fields){
            value = value + " " + fields[frag_field.get_field_name()] + ",";
        }
        value = value.substr(0, value.length() - 1);
        // LOG(INFO) << site_address << " : " << value;
        values[site_address] = value;
    }
    return values;
}

void insert_data(const std::string &table_name, const std::string data) {
    std::cout << SEPERATE_LINE << std::endl;
    LOG(INFO) << "Insert data " << data <<  " into table " << table_name << ".";
    std::string table_string = read_kv_from_etcd(table_name);
    if(table_string == "") {
        LOG(ERROR) << "No table named " << table_name;
        return;
    }
    nlohmann::json table_json = nlohmann::json::parse(table_string);
    LOG(INFO) << table_json.dump();
    TableMeta table(table_json);
    std::string create_sql = table.get_create_table_sql();
    LOG(INFO) << "create frag sql: "<< create_sql;
    std::string fields_meta = create_sql.substr(create_sql.find_first_of('(')+1);
    trim(fields_meta);
    fields_meta.erase(fields_meta.length()-2);
    LOG(INFO) << "fields_meta: " << fields_meta;
    std::vector<std::string> fields_name;
    std::vector<std::string> types;

    for(Field field : table.get_table_fields()) {
        fields_name.push_back(field.get_field_name());
        types.push_back(field.get_field_type());
    }
    int frags_num = table.get_fragments().size();
    bool is_horizontal = table.get_fragments().begin()->frag_is_horizontal();

    std::string tuple = data.substr(data.find_first_of('(')+1, data.find_last_of(')')-1);
    std::istringstream sin(tuple);
    std::string field;
    if(is_horizontal) {
        LOG(INFO) << "Insert into horizontal frags: " << tuple;
        std::vector<std::string> fields;
        std::string field;
        while(std::getline(sin, field, ',')){
            trim(field);
            fields.push_back(field);
        }
        LOG(INFO) << field;
        std::string host = get_insert_site(table_name, fields_name, fields);
        LOG(INFO) << host;
        std::string value = create_insert_value_for_horizontal(fields, types);
        LOG(INFO) << value;
         std::vector<std::string> values;
        values.push_back(value);
        LOG(INFO) << host << " : " << value;
        import_data(host, table_name, fields_meta, values);
    }
    else {
        LOG(INFO) << "Insert into vertical frags: " << tuple;
        std::map<std::string, std::string> fields;
        auto field_name = fields_name.begin();
        std::string field;
        while(std::getline(sin, field, ',')){
            std::string name = *field_name;
            trim(field);
            fields[name] = field;
            LOG(INFO) << name << " : " << field;
            field_name++;
        }

        std::istringstream sin1(fields_meta);
        std::map<std::string, std::string> metas;
        std::string meta;
        field_name = fields_name.begin();
        while(field_name != fields_name.end() && std::getline(sin1, meta, ',')){
            std::string name = *field_name;
            trim(meta);
            metas[name] = meta;
            LOG(INFO) << name << " : " << meta;
            field_name++;
        }
        LOG(INFO) << fields_meta;
        std::map<std::string, std::string> value_map = create_insert_value_for_vertical(table, fields);
        std::map<std::string, std::string> meta_map = create_insert_value_for_vertical(table, fields);

        for(auto meta_it = meta_map.begin(); meta_it != meta_map.end(); meta_it++) {
            std::vector<std::string> values;
            LOG(INFO) << meta_map[meta_it->first] << ": " << meta_it->second;
            values.push_back(value_map[meta_it->first]);
            import_data(meta_it->first, table_name, meta_map[meta_it->first], values);
        }
    }
    LOG(INFO) << "Data " << data << " has been inserted to table " << table_name << ".";
}

void delete_data(const std::string &table_name, const std::string delete_predicate, std::map<std::string, std::string> predicates) {
    std::cout << SEPERATE_LINE << std::endl;
    LOG(INFO) << "Start deleting data where " << delete_predicate << " from table " << table_name << ".";
    std::string table_string = read_kv_from_etcd(table_name);
    if(table_string == "") {
        LOG(ERROR) << "No table named " << table_name;
        return;
    }
    nlohmann::json table_json = nlohmann::json::parse(table_string);
    TableMeta table(table_json);
    std::vector<std::string> fields_name;
    std::vector<std::string> types;

    // for(Field field : table.get_table_fields()) {
    //     fields_name.push_back(field.get_field_name());
    //     types.push_back(field.get_field_type());
    // }
    int frags_num = table.get_fragments().size();
    bool is_horizontal = table.get_fragments().begin()->frag_is_horizontal();

    if(is_horizontal) {
        std::vector<std::string> hosts;
        for(auto frag:table.get_fragments()) {
            std::string frag_cond = frag.get_frag_predicate();
            if(conflict(delete_predicate,frag_cond)) {
               continue;
            }
            hosts.push_back(frag.get_frag_site_address());
        }
        std::string delete_sql = "delete from " +  table_name + " where " + delete_predicate;
        for(std::string host : hosts){
            execute_non_query_sql(host, delete_sql);
        }
    }
    else {
        std::string frag_on = table.get_fragments().begin()->get_frag_field().get_field_name();
        Node delete_node(0, "", ("temp_table1." + frag_on + " = temp_table2." + frag_on), "temp_table1."+frag_on, NODE_NOT_LEAF, false);
        std::map<std::string, std::string> predicate;
        int i = 0;
        for(auto frag : table.get_fragments()) {
            std::vector<Field> fields = frag.get_fields();
            std::string select_predicate = "";
            for(auto field : fields) {
                if(predicates[field.get_field_name()] != "") {
                    select_predicate = select_predicate + predicates[field.get_field_name()] + " and ";
                }
            }
            select_predicate = select_predicate.substr(0, select_predicate.rfind("and"));
            trim(select_predicate);
            if(select_predicate != ""){
                // LOG(INFO) << "select of node " << frag.get_frag_name() << " " << select_predicate;
            }
            Node select_node(0, select_predicate, "", frag_on, NODE_LEAF, false);
            
            std::map<std::string, std::string> child = {{table_name, frag.get_frag_site_address()}};
            select_node.set_children(child);
            // LOG(INFO) << "node " << frag.get_frag_name() << " " << select_node.convert_to_json().dump();
            std::string temp_table_name = "temp_table" + std::to_string(i+1);
            i++;
            write_to_etcd(temp_table_name, select_node.convert_to_json());
        }
        std::map<std::string, std::string> child = {{"temp_table1", table.get_fragments()[0].get_frag_site_address()}, {"temp_table2", table.get_fragments()[1].get_frag_site_address()}};
        delete_node.set_children(child);
        write_to_etcd("delete_node", delete_node.convert_to_json());
        show_plan_tree("delete_node");

        std::vector<std::string> delete_ids = request_node("delete_node", table.get_fragments()[0].get_frag_site_address());
        std::string delete_sql = "delete from " + table_name + " where ";
        for(std::string id : delete_ids) {
            delete_sql = delete_sql + frag_on + " = " + id + " and ";
        }
        delete_sql = delete_sql.substr(0, delete_sql.rfind("and"));
        delete_sql += ";";
        for(auto frag : table.get_fragments()) {
            execute_non_query_sql(frag.get_frag_site_address(), delete_sql);
        }
    }
}

void load_data(const std::string &table_name, const std::string &data_file_name) {
    std::cout << SEPERATE_LINE << std::endl;
    LOG(INFO) << "Start loading data in file" << data_file_name << " into table " << table_name << ".";
    std::string table_string = read_kv_from_etcd(table_name);
    if(table_string == "") {
        LOG(ERROR) << "No table named " << table_name;
        return;
    }
    nlohmann::json table_json = nlohmann::json::parse(table_string);
    TableMeta table(table_json);
    std::string create_sql = table.get_create_table_sql();
    std::string fields_meta = create_sql.substr(create_sql.find_first_of('(')+1, create_sql.find_last_of(')'));
    std::vector<std::string> fields_name;
    std::vector<std::string> types;

    for(Field field : table.get_table_fields()) {
        fields_name.push_back(field.get_field_name());
        types.push_back(field.get_field_type());
    }
    int frags_num = table.get_fragments().size();
    bool is_horizontal = table.get_fragments().begin()->frag_is_horizontal();

    std::map<std::string, std::vector<std::string>> values;

    std::ifstream infile(data_file_name);
    std::string line = "";
    int row_count = 0;
    while(std::getline(infile, line)){
        std::istringstream sin(line);
        if(is_horizontal) {
            std::vector<std::string> fields;
            std::string field;
            int id = 0;
            while(std::getline(sin, field, '\t')){
                fields.push_back(field);
            }
            std::string host = get_insert_site(table_name, fields_name, fields);
            std::string value = create_insert_value_for_horizontal(fields, types);
            // LOG(INFO) << host << " : " << value;
            values[host].push_back(value);
        }
        else {
            std::map<std::string, std::string> fields;
            auto field_name = fields_name.begin();
            auto field_type = types.begin();
            std::string field;
            while(std::getline(sin, field, '\t')){
                std::string name = *field_name;
                if(*field_type == "INT") {
                    fields[name] = field;
                }
                else {
                    fields[name] = "\'" + field + "\'";
                }
                field_name++;
                field_type++;
            }
            
            std::map<std::string, std::string> value_map = create_insert_value_for_vertical(table, fields);
            for(auto value_it = value_map.begin(); value_it != value_map.end(); value_it++) {
                values[value_it->first].push_back(value_it->second);
            }
        }
        row_count++;
    }
    for(auto values_it = values.begin(); values_it != values.end(); values_it++) {
        import_data(values_it->first, table_name, fields_meta, values_it->second);
    }
    LOG(INFO) << "All data in file" << data_file_name << " has been imported to table " << table_name << ".";
    LOG(INFO) << "Total rows: " << row_count;
}

void insert_sql_test(){
    insert_data("customer", "(300001, Xiaoming, 1)");
    insert_data("book", "(206000, DDB, Oszu, 104001, 100)");
    insert_data("publisher", "(104001,High Education Press, PRC)");
    insert_data("customer", "(300002, Xiaohong, 1)");
    insert_data("orders", "(300001, 205001,5)");
}

void delete_data_test(){
    std::string table_name = "customer";
    std::string delete_predicate = "customer_rank=1";
    std::map<std::string, std::string> predicates = {{"customer_id", ""}, {"customer_name", ""}, {"customer_rank", "customer_rank=1"}};
    delete_data(table_name, delete_predicate, predicates);
    std::string book_table_name = "book";
    std::string book_delete_predicate = "book_copies=100";
    std::map<std::string, std::string> book_predicates = {{"book_id", ""}, {"book_title", ""}, {"book_authors", ""}, {"book_publisher_id", ""}, {"book_copies", "book_copies=100"}};
    delete_data(book_table_name, book_delete_predicate, book_predicates);
}

void create_frag_test(){
    LOG(INFO) << "Create frag test:";
    Field book_id("INT", "book_id");
    Field book_title("VARCHAR", "book_title");
    Field book_authors("VARCHAR", "book_authors");
    Field book_copies("INT", "book_copies");
    std::vector<Field> book_fields;
    book_fields.push_back(book_id);
    book_fields.push_back(book_title);
    book_fields.push_back(book_authors);
    book_fields.push_back(book_copies);
    std::string book_frag_predicate1 = "book_id < 205000";
    std::string book_frag_predicate2 = "book_id >= 205000 and book_id < 210000";
    std::string book_frag_predicate3 = "book_id >= 210000";
    std::string book_create = "create table `book` (`book_id` int not null, `book_title` varchar(100) not null, `book_authors` varchar(200) not null, `book_publisher_id` int not null, `book_copies` int not null, primary key(`book_id`));";
    FragmentMeta frag_book1("book.1", "book", "192.168.122.66:8000", "site1", book_fields, true, book_id, book_frag_predicate1, book_create);
    FragmentMeta frag_book2("book.2", "book", "192.168.122.67:8000", "site2", book_fields, true, book_id, book_frag_predicate2, book_create);
    FragmentMeta frag_book3("book.3", "book", "192.168.122.68:8000", "site3", book_fields, true, book_id, book_frag_predicate3, book_create);
    create_fragment(frag_book1);
    create_fragment(frag_book2);
    create_fragment(frag_book3);


    Field customer_id("INT", "customer_id");
    Field customer_id2("INT", "customer_id");
    Field customer_name("VARCHAR", "customer_name");
    Field customer_rank("INT", "customer_rank");
    std::vector<Field> customer_fields1;
    std::vector<Field> customer_fields2;
    customer_fields1.push_back(customer_id);
    customer_fields2.push_back(customer_id2);
    customer_fields1.push_back(customer_name);
    customer_fields2.push_back(customer_rank);
    std::string customer1_create = "create table `customer` (`customer_id` int, `customer_name` varchar(25), primary key(`customer_id`));";
    std::string customer2_create = "create table `customer` (`customer_id` int, `customer_rank` int, primary key(`customer_id`));";
    FragmentMeta frag_customer1("customer.1", "customer", "192.168.122.66:8000", "site1", customer_fields1, false, customer_id, "", customer1_create);
    FragmentMeta frag_customer2("customer.2", "customer", "192.168.122.67:8000", "site2", customer_fields2, false, customer_id2, "", customer2_create);
    create_fragment(frag_customer1);
    create_fragment(frag_customer2);

    Field publisher_id("INT", "publisher_id");
    Field publisher_name("VARCHAR", "publisher_name");
    Field publisher_nation("VARCHAR", "publisher_nation");
    std::vector<Field> publisher_fields;
    publisher_fields.push_back(publisher_id);
    publisher_fields.push_back(publisher_name);
    publisher_fields.push_back(publisher_nation);
    std::string publisher_frag_predicate1 = "publisher_id < 104000 AND publisher_nation= 'PRC'";
    std::string publisher_frag_predicate2 = "publisher_id < 104000 AND publisher_nation='USA'";
    std::string publisher_frag_predicate3 = "publisher_id >= 104000 AND publisher_nation='PRC'";
    std::string publisher_frag_predicate4 = "publisher_id >= 104000 AND publisher_nation='USA'";

    std::string publisher_create = "create table `publisher` (`publisher_id` int not null, `publisher_name` varchar(100) not null, `publisher_nation` varchar(3) not null, primary key(`publisher_id`));";
    FragmentMeta frag_publisher1("publisher.1", "publisher", "192.168.122.66:8000", "site1", publisher_fields, true, publisher_id, publisher_frag_predicate1, publisher_create);
    FragmentMeta frag_publisher2("publisher.2", "publisher", "192.168.122.67:8000", "site2", publisher_fields, true, publisher_id, publisher_frag_predicate2, publisher_create);
    FragmentMeta frag_publisher3("publisher.3", "publisher", "192.168.122.68:8000", "site3", publisher_fields, true, publisher_id, publisher_frag_predicate3, publisher_create);
    FragmentMeta frag_publisher4("publisher.4", "publisher", "192.168.122.66:7999", "site4", publisher_fields, true, publisher_id, publisher_frag_predicate4, publisher_create);

    create_fragment(frag_publisher1);
    create_fragment(frag_publisher2);
    create_fragment(frag_publisher3);
    create_fragment(frag_publisher4);
    
    Field orders_customer_id("INT", "orders_customer_id");
    Field orders_book_id("INT", "orders_book_id");
    Field orders_quantity("INT", "orders_quantity");
    std::vector<Field> orders_fields;
    orders_fields.push_back(orders_customer_id);
    orders_fields.push_back(orders_book_id);
    orders_fields.push_back(orders_quantity);
    std::string orders_frag_predicate1 = "orders_customer_id < 307000 and orders_book_id < 215000";
    std::string orders_frag_predicate2 = "orders_customer_id < 307000 and orders_book_id >= 215000";
    std::string orders_frag_predicate3 = "orders_customer_id >= 307000 and orders_book_id < 215000";
    std::string orders_frag_predicate4 = "orders_customer_id >= 307000 and orders_book_id >= 215000";

    std::string orders_create = "create table `orders` (`orders_customer_id` int not null, `orders_book_id` int not null, `orders_quantity` int not null);";
    FragmentMeta frag_orders1("orders.1", "orders", "192.168.122.66:8000", "site1", orders_fields, true, orders_customer_id, orders_frag_predicate1, orders_create);
    FragmentMeta frag_orders2("orders.2", "orders", "192.168.122.67:8000", "site2", orders_fields, true, orders_customer_id, orders_frag_predicate2, orders_create);
    FragmentMeta frag_orders3("orders.3", "orders", "192.168.122.68:8000", "site3", orders_fields, true, orders_customer_id, orders_frag_predicate3, orders_create);
    FragmentMeta frag_orders4("orders.4", "orders", "192.168.122.66:7999", "site4", orders_fields, true, orders_customer_id, orders_frag_predicate4, orders_create);

    create_fragment(frag_orders1);
    create_fragment(frag_orders2);
    create_fragment(frag_orders3);
    create_fragment(frag_orders4);

    show_fragments_of_site("site1");
    show_fragments_of_site("site2");
    show_fragments_of_site("site3");
    show_fragments_of_site("site4");

    show_fragments_of_table("book");
    show_fragments_of_table("customer");
    show_fragments_of_table("publisher");
    show_fragments_of_table("orders");
}

void create_site_test(){
    LOG(INFO) << "Create site test:";
    create_site("site1", "192.168.122.66:8000");
    create_site("site2", "192.168.122.67:8000");
    create_site("site3", "192.168.122.68:8000");
    create_site("site4", "192.168.122.66:7999");
    show_sites();
}

void create_table_test(){
    LOG(INFO) << "Create table test:";
    Field book_id("INT", "book_id");
    Field book_title("VARCHAR", "book_title");
    Field book_authors("VARCHAR", "book_authors");
    Field book_copies("INT", "book_copies");
    std::vector<Field> book_fields;
    book_fields.push_back(book_id);
    book_fields.push_back(book_title);
    book_fields.push_back(book_authors);
    book_fields.push_back(book_copies);
    TableMeta table_book(std::string("book"), 0, book_fields, 0, "create table `book` (`book_id` int not null, `book_title` varchar(100) not null, `book_authors` varchar(200) not null, `book_publisher_id` int not null, `book_copies` int not null, primary key(`book_id`));");
    create_table(table_book);

    Field customer_id("INT", "customer_id");
    Field customer_name("VARCHAR", "customer_name");
    Field customer_rank("INT", "customer_rank");
    std::vector<Field> customer_fields;
    customer_fields.push_back(customer_id);
    customer_fields.push_back(customer_name);
    customer_fields.push_back(customer_rank);
    TableMeta table_customer(std::string("customer"), 0, customer_fields, 0, "create table `customer` (`customer_id` int, `customer_name` varchar(25), `customer_rank` int, primary key(`customer_id`));");
    create_table(table_customer);

    Field orders_c_id("INT", "orders_customer_id");
    Field orders_b_id("INT", "orders_book_id");
    Field orders_quantity("INT", "orders_quantity");
    std::vector<Field> orders_fields;
    orders_fields.push_back(orders_c_id);
    orders_fields.push_back(orders_b_id);
    orders_fields.push_back(orders_quantity);
    TableMeta table_orders(std::string("orders"), 0, orders_fields, 0, "create table `orders` (`orders_customer_id` int, `orders_book_id` int, `orders_quantity` int);");
    create_table(table_orders);

    Field publisher_id("INT", "publisher_id");
    Field publisher_name("VARCHAR", "publisher_name");
    Field publisher_nation("VARCHAR", "publisher_nation");
    std::vector<Field> publisher_fields;
    publisher_fields.push_back(publisher_id);
    publisher_fields.push_back(publisher_name);
    publisher_fields.push_back(publisher_nation);
    TableMeta table_publisher(std::string("publisher"), 0, publisher_fields, 0, "create table `publisher` (`publisher_id` int key, `publisher_name` varchar(100), `publisher_nation` varchar(3), primary key(`publisher_id`));");
    create_table(table_publisher);
    show_tables();
}



int main(int argc, char* argv[]){

// Parse gflags. We recommend you to use gflags as well.
    GFLAGS_NS::ParseCommandLineFlags(&argc, &argv, true);
    if(FLAGS_init_all == true) {
        LOG(INFO) << "Init etcd service";
        LOG(INFO) << "Delete tables";
        LOG(INFO) << "Delete sites";
        delete_kv_from_etcd("tables");
        delete_kv_from_etcd("sites");
        create_site_test();
        create_table_test();
        create_frag_test();
        if(FLAGS_load){
            load_data("book", "data/book.tsv");
            load_data("customer", "data/customer.tsv");
            load_data("orders", "data/orders.tsv");
            load_data("publisher", "data/publisher.tsv");
        } 
    }
    LOG(INFO) << "start service";
    std::string host = std::string("127.0.0.1") + std::string(":") + std::to_string(FLAGS_port_server);

    if(FLAGS_test_insert){
        insert_sql_test();
    }
    else if(FLAGS_test_delete){
        delete_data_test();
    }
    // insert_sql_test();
    // delete_data_test();
    // load_data("book", "data/book.tsv");
    // load_data("customer", "data/customer.tsv");

    if(FLAGS_test_sql){
        LOG(INFO) << "sql to execute: " << FLAGS_sql;
        
        std::string root_table = parsesql(FLAGS_sql);
        show_plan_tree(root_table);

        std::vector<std::string> result = request_node(root_table, host);
        LOG(INFO) << "sql finished";
        if(FLAGS_verbose){
            for(auto tuple : result) {
                std::cout << "\t" << SEPERATE_SIGN << tuple << "\t" << SEPERATE_SIGN << std::endl;
            }
        }
    }

    // delete_table(host, table_name);
    // delete_table(host, table_name1);

    // return request_node("query_0_0_temp_table", "127.0.0.1:8000");
    // test_query_7(host);

    // test_query_7_with_2nodes(host);

    return 0;
}